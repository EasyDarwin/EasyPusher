package com.android.webrtc.audio;

import android.content.Context;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaRecorder;
import android.media.audiofx.AudioEffect;
import android.media.audiofx.AutomaticGainControl;
import android.media.audiofx.EnvironmentalReverb;
import android.media.audiofx.LoudnessEnhancer;
import android.os.Build;
import android.os.Process;
import android.preference.PreferenceManager;
import android.util.Log;


import com.android.webrtc.audio.MobileAEC;

import org.easydarwin.easypusher.BuildConfig;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PipedInputStream;
import java.io.PipedOutputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.ShortBuffer;

/**
 * Created by John on 2016/11/12.
 */

public class AudioIO {

    private static final String TAG = "AudioIO";
    private final Context mContext;
    private final boolean mStereo;
    private final int mSample;
    private final int mAudioFormat;
    private AudioTrack mAudioTrack;
    private AudioEffect[] mAes = new AudioEffect[10];
    private AudioRecord mAudioRecoder;
    private MobileAEC aecm;

    private PipedOutputStream os_farend;
    private PipedInputStream is_farend;


    private PipedOutputStream os_nearend;
    private PipedInputStream is_nearend;

    private class AudioThread extends Thread {

        public AudioThread() {
            super("AudioIO");
        }

        @Override
        public void run() {
            Process.setThreadPriority(Process.THREAD_PRIORITY_AUDIO);
            aecm = new MobileAEC(MobileAEC.SamplingFrequency.FS_8000Hz);
            if (BuildConfig.DEBUG) {
                aecm.setAecmMode(getMode()).prepare();
            } else {
                aecm.setAecmMode(MobileAEC.AggressiveMode.HIGH).prepare();
            }

            final int sampleRateInHz = mSample;
            final int channelConfig = mStereo ? AudioFormat.CHANNEL_OUT_STEREO : AudioFormat.CHANNEL_OUT_MONO;
            final int bfSize = AudioTrack.getMinBufferSize(sampleRateInHz, channelConfig, mAudioFormat);

            // 10毫秒内的字节数
            final int unit_length = sampleRateInHz * 10 / 1000 * 2;

            mAudioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, sampleRateInHz, channelConfig, mAudioFormat, bfSize * 2, AudioTrack.MODE_STREAM);
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
                int i = 0;
                try {
                    EnvironmentalReverb er = new EnvironmentalReverb(0, mAudioTrack.getAudioSessionId());
                    if (er != null) {
                        er.setEnabled(true);
                        mAes[i++] = er;
                    }
                } catch (Throwable ex) {
                    ex.printStackTrace();
                }
                try {
                    LoudnessEnhancer le = new LoudnessEnhancer(mAudioTrack.getAudioSessionId());
                    le.setEnabled(true);
                    mAes[i++] = le;
                } catch (Throwable ex) {
                    ex.printStackTrace();
                }
            }
            mAudioTrack.play();


            int CC = AudioFormat.CHANNEL_IN_MONO;
            int minBufSize = AudioRecord.getMinBufferSize(sampleRateInHz, CC, mAudioFormat);
            final int audioSource = MediaRecorder.AudioSource.MIC;
            // 初始化时，这个参数不是越小越好。这个参数(应该)是底层的音频buffer的尺寸，如果太小了，又读取不及时，可能会溢出，导致音质不好
            minBufSize *= 2;
            if (minBufSize < unit_length) {
                minBufSize = unit_length;
            }
            mAudioRecoder = new AudioRecord(audioSource, sampleRateInHz, CC, mAudioFormat, minBufSize);
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
                int i = 0;
                try {
                    AutomaticGainControl er = AutomaticGainControl.create(mAudioRecoder.getAudioSessionId());
                    if (er != null) {
                        er.setEnabled(true);
                        mAes[i++] = er;
                    }
                } catch (Throwable ex) {
                    ex.printStackTrace();
                }
            }
            mAudioRecoder.startRecording();

            byte[] buffer = new byte[unit_length];
            int sizeInShorts = unit_length / 2;
            short[] farendPCM = new short[sizeInShorts];

            short[] nearendPCM = new short[sizeInShorts];
            short[] nearendCanceled = new short[sizeInShorts];
            ByteBuffer bb = ByteBuffer.allocate(unit_length).order(ByteOrder.LITTLE_ENDIAN);

            try {
                while (t != null) {
                    boolean fillfarend = fillFarendBuffer(buffer);
                    if (fillfarend) {
                        save(buffer, "/sdcard/farend.pcm", true);
                        ByteBuffer.wrap(buffer).order(ByteOrder.LITTLE_ENDIAN).asShortBuffer().get(farendPCM);
                        mAudioTrack.write(farendPCM, 0, sizeInShorts);
                    }
                    if (readNearendBuffer(nearendPCM)) {
                        bb.clear();
                        if (fillfarend) {
                            int delay_level = PreferenceManager.getDefaultSharedPreferences(mContext).getInt("delay_level", 3);
                            if (BuildConfig.DEBUG) {
                                try {
                                    delay_level = getDelay();
                                } catch (Exception ex) {
                                }
                            }
                            aecm.farendBuffer(farendPCM, sizeInShorts);
                            bb.asShortBuffer().put(nearendPCM);
                            save(bb.array(), "/sdcard/nearend.pcm", true);
                            aecm.echoCancellation(nearendPCM, null, nearendCanceled, (short) (sizeInShorts), (short) (10 * delay_level));

                            bb.clear();
                            bb.asShortBuffer().put(nearendCanceled);
                            save(bb.array(), "/sdcard/nearendCanceled.pcm", true);
                            os_nearend.write(bb.array());
                        } else {
                            bb.asShortBuffer().put(nearendPCM);
                            save(bb.array(), "/sdcard/nearend.pcm", true);
                            os_nearend.write(bb.array());
                        }
                    }
                }
            } catch (IOException e) {
                e.printStackTrace();
            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                mAudioTrack.release();
                mAudioRecoder.release();

                for (Object ae : mAes) {
                    if (ae != null) {
                        AudioEffect aet = (AudioEffect) ae;
                        aet.release();
                    }
                }
            }
        }

        private boolean readNearendBuffer(short[] pcm) {
            int offset = 0;
            do {
                int i = mAudioRecoder.read(pcm, offset, pcm.length - offset);
                if (i < 0 || t == null) return false;
                offset += i;
            } while (offset < pcm.length);
            if (BuildConfig.DEBUG) {
                Log.d(TAG, String.format("readNearendBuffer : %d", pcm.length));
            }
            return true;
        }

        private MobileAEC.AggressiveMode getMode() {
            try {
//                String[] arr = AddVideoOverlay.AddText.split("_");
                // 如果为偶数，不读远端了。
                int delay_level = 1;
                return new MobileAEC.AggressiveMode(delay_level);
            } catch (Exception ex) {
                return MobileAEC.AggressiveMode.HIGH;
            }
        }


        private int getDelay() {
            try {
//                String[] arr = AddVideoOverlay.AddText.split("_");
                // 如果为偶数，不读远端了。
//                int delay_level = Integer.parseInt(10);
                return 10;
            } catch (Exception ex) {
                return 1;
            }
        }

        private boolean fillFarendBuffer(byte[] bufferPCM) throws IOException {
            if (is_farend.available() < 1) {
                return false;
            }
            int left = bufferPCM.length;
            do {
                int i = is_farend.read(bufferPCM, bufferPCM.length - left, left);
                if (i < 0 || t == null) return false;
                left -= i;
            } while (left > 0);
            if (BuildConfig.DEBUG) {
                Log.d(TAG, String.format("fillFarendBuffer : %d", bufferPCM.length));
            }
            return true;
        }
    }

    private Thread t;

    public AudioIO(Context context, int sample, boolean stereo) {
        this(context, sample, stereo, AudioFormat.ENCODING_PCM_16BIT);
    }

    public AudioIO(Context context, int sample, boolean stereo, int audioFormat) {
        mContext = context.getApplicationContext();
        mSample = sample;
        mStereo = stereo;
        mAudioFormat = audioFormat;
    }

    public synchronized void start() throws IOException {
        if (t != null) {
            return;
        }

        is_farend = new PipedInputStream(1024);
        os_farend = new PipedOutputStream(is_farend);

        is_nearend = new PipedInputStream(1024);
        os_nearend = new PipedOutputStream(is_nearend);
        t = new AudioThread();
        t.start();
    }

    public synchronized void release() throws IOException, InterruptedException {
        if (is_farend != null) {
            is_farend.close();
        }
        if (os_nearend != null)
            os_nearend.close();
        if (is_nearend != null)
            is_nearend.close();
        if (os_farend != null)
            os_farend.close();
        Thread t = this.t;
        this.t = null;
        if (t != null) {
            t.interrupt();
            t.join();
        }

    }

    public void pumpAudio(short[] pcm, int offset, int length) throws InterruptedException, IOException {
        ByteBuffer bb = ByteBuffer.allocate(length * 2).order(ByteOrder.LITTLE_ENDIAN);
        ShortBuffer sb = bb.asShortBuffer();
        sb.put(pcm, offset, length);

        os_farend.write(bb.array());
    }

    public int retrieveAudio(byte[] pcm, int offset, int length) throws IOException {
        return is_nearend.read(pcm, offset, length);
    }

    public static void save(byte[] buffer, String path, boolean append) {
        FileOutputStream fos = null;
        try {
            fos = new FileOutputStream(path, append);
            fos.write(buffer);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            if (fos != null) {
                try {
                    fos.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }


}
