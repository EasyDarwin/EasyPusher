package org.easydarwin.audio;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.media.MediaRecorder;
import android.os.Build;
import android.util.Log;
import android.os.Process;
import org.easydarwin.push.EasyPusher;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.nio.ByteBuffer;

/**
 * Created by Helong on 16/3/26-14:31.
 */
public class AudioStream {

    private int samplingRate = 8000;
    private int bitRate = 16000;
    private int BUFFER_SIZE = 1600;
    int mSamplingRateIndex=0;
    AudioRecord mAudioRecord;
    MediaCodec mMediaCodec;
    EasyPusher easyPusher;
    private Thread mThread = null;
    private Thread encodeThread = null;
    String TAG = "audio_stream";
    //final String path = Environment.getExternalStorageDirectory() + "/123450001.aac";
    boolean stoped = false;

    protected MediaCodec.BufferInfo mBufferInfo = new MediaCodec.BufferInfo();
    protected ByteBuffer[] mBuffers = null;
    protected ByteBuffer mBuffer = null;
    protected int mIndex = -1;

    /** There are 13 supported frequencies by ADTS. **/
    public static final int[] AUDIO_SAMPLING_RATES = { 96000, // 0
            88200, // 1
            64000, // 2
            48000, // 3
            44100, // 4
            32000, // 5
            24000, // 6
            22050, // 7
            16000, // 8
            12000, // 9
            11025, // 10
            8000, // 11
            7350, // 12
            -1, // 13
            -1, // 14
            -1, // 15
    };

    public AudioStream(EasyPusher easyPusher) {
        this.easyPusher = easyPusher;
        int i = 0;
        for (; i < AUDIO_SAMPLING_RATES.length; i++) {
            if (AUDIO_SAMPLING_RATES[i] == samplingRate) {
                mSamplingRateIndex = i;
                break;
            }
        }
    }

    private void init() {
        try {
            stoped=false;
            int bufferSize = AudioRecord.getMinBufferSize(samplingRate,
                    AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT) * 16;
            mAudioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC,
                    samplingRate, AudioFormat.CHANNEL_IN_MONO,
                    AudioFormat.ENCODING_PCM_16BIT, bufferSize);
            mMediaCodec = MediaCodec.createEncoderByType("audio/mp4a-latm");
            MediaFormat format = new MediaFormat();
            format.setString(MediaFormat.KEY_MIME, "audio/mp4a-latm");
            format.setInteger(MediaFormat.KEY_BIT_RATE, bitRate);
            format.setInteger(MediaFormat.KEY_CHANNEL_COUNT, 1);
            format.setInteger(MediaFormat.KEY_SAMPLE_RATE, samplingRate);
            format.setInteger(MediaFormat.KEY_AAC_PROFILE,
                    MediaCodecInfo.CodecProfileLevel.AACObjectLC);
            format.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, BUFFER_SIZE);
            mMediaCodec.configure(format, null, null,
                    MediaCodec.CONFIGURE_FLAG_ENCODE);
        } catch (Exception e) {

        }
    }

    private boolean AACStreamingSupported() {
        if (Build.VERSION.SDK_INT < 14)
            return false;
        try {
            MediaRecorder.OutputFormat.class.getField("AAC_ADTS");
            return true;
        } catch (Exception e) {
            return false;
        }
    }

    /**
     * 编码
     */
    private void startEncode() {
        mBuffers = mMediaCodec.getOutputBuffers();
        mBuffer=null;
        encodeThread = new Thread(new Runnable() {
            @Override
            public void run() {
                while (!Thread.interrupted() && !stoped) {
                    try {
                        if (mBuffer == null) {
                            mBuffer = ByteBuffer.allocate(10240);
                            while (!Thread.currentThread().isInterrupted() && !stoped) {
                                mIndex = mMediaCodec.dequeueOutputBuffer(mBufferInfo, 50000);
                                if (mIndex >= 0) {
                                    if (mBufferInfo.flags == MediaCodec.BUFFER_FLAG_CODEC_CONFIG) {
                                        continue;
                                    }
                                    mBuffer.clear();
                                    mBuffer.position(7);
                                    mBuffers[mIndex].get(mBuffer.array(), 7, mBufferInfo.size);
                                    mBuffers[mIndex].clear();
                                    mBuffer.position(mBuffer.position() + mBufferInfo.size);
                                    addADTStoPacket(mBuffer.array(), mBufferInfo.size + 7);
                                    mBuffer.flip();
                                    break;
                                } else if (mIndex == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
                                    mBuffers = mMediaCodec.getOutputBuffers();
                                } else if (mIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                                    Log.v(TAG, "output format changed...");
                                } else if (mIndex == MediaCodec.INFO_TRY_AGAIN_LATER) {
                                    Log.v(TAG, "No buffer available...");
                                } else {
                                    Log.e(TAG, "Message: " + mIndex);
                                }
                            }
                        }
                        int size = mBufferInfo.size + 7;
                        byte[] buffer = new byte[size];
                        mBuffer.get(buffer);
                        easyPusher.push(buffer,System.currentTimeMillis(), 0);
                        if (mBuffer.position() >= size) { // read complete
                            mMediaCodec.releaseOutputBuffer(mIndex, false);
                            mBuffer = null;
                        }
                    } catch (RuntimeException e) {
                        StringWriter sw = new StringWriter();
                        PrintWriter pw = new PrintWriter(sw);
                        e.printStackTrace(pw);
                        String stack = sw.toString();
                        Log.i(TAG, "record" + stack);
                    }
                }
            }
        }, "AACEncoder");
        encodeThread.start();
    }


    public void startRecord() {
        try {
            init();
            mAudioRecord.startRecording();
            mMediaCodec.start();
            final ByteBuffer[] inputBuffers = mMediaCodec.getInputBuffers();
            mThread = new Thread(new Runnable() {
                @Override
                public void run() {
                    Process.setThreadPriority(Process.THREAD_PRIORITY_AUDIO);
                    int len = 0, bufferIndex = 0;
                    try {
                        while (!Thread.interrupted() && !stoped) {

                            bufferIndex = mMediaCodec.dequeueInputBuffer(50000);
                            if (bufferIndex >= 0) {
                                inputBuffers[bufferIndex].clear();
                                len = mAudioRecord.read(inputBuffers[bufferIndex], BUFFER_SIZE);
                                if (len == AudioRecord.ERROR_INVALID_OPERATION || len == AudioRecord.ERROR_BAD_VALUE) {
                                    mMediaCodec.queueInputBuffer(bufferIndex, 0, 0, 0, 0);
                                } else {
                                    mMediaCodec.queueInputBuffer(bufferIndex, 0, len, 0, 0);
                                }
                            }
                        }
                    } catch (RuntimeException e) {
                        StringWriter sw = new StringWriter();
                        PrintWriter pw = new PrintWriter(sw);
                        e.printStackTrace(pw);
                        String stack = sw.toString();
                        Log.i(TAG, "record" + stack);

                    }
                }
            }, "AACRecoder");
            mThread.start();
            startEncode();
        } catch (Exception e) {
            Log.e(TAG, "Record___Error!!!!!");
        }
    }

    private void addADTStoPacket(byte[] packet, int packetLen) {
        packet[0] = (byte) 0xFF;
        packet[1] = (byte) 0xF1;
        packet[2] = (byte) (((2 - 1) << 6) + (mSamplingRateIndex << 2) + (1 >> 2));
        packet[3] = (byte) (((1 & 3) << 6) + (packetLen >> 11));
        packet[4] = (byte) ((packetLen & 0x7FF) >> 3);
        packet[5] = (byte) (((packetLen & 7) << 5) + 0x1F);
        packet[6] = (byte) 0xFC;
    }

    public void stop() {
        try {

            if (mThread != null) {
                mThread.interrupt();
            }
            if (encodeThread != null) {
                encodeThread.interrupt();
            }
            stoped = true;
            if (mAudioRecord != null) {
                mAudioRecord.stop();
                mAudioRecord.release();
                mAudioRecord = null;
            }
            if (mMediaCodec != null) {
                mMediaCodec.stop();
                mMediaCodec.release();
                mMediaCodec = null;
            }
        } catch (Exception e) {
            Log.e(TAG, "Stop___Error!!!!!");
        }
    }

}
