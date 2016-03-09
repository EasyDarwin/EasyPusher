/*
 * Copyright (C) 2011-2014 GUIGUI Simon, fyhertz@gmail.com
 * 
 * This file is part of Spydroid (http://code.google.com/p/spydroid-ipcamera/)
 * 
 * Spydroid is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this source code; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

package org.easydarwin.hw;

import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.nio.ByteBuffer;
import java.util.Arrays;

import org.easydarwin.hw.CodecManager.Codec;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.media.MediaCodec;
import android.media.MediaCodec.BufferInfo;
import android.media.MediaFormat;
import android.os.Build;
import android.preference.PreferenceManager;
import android.util.Base64;
import android.util.Log;

/**
 * The purpose of this class is to detect and by-pass some bugs (or
 * underspecified configuration) that encoders available through the MediaCodec
 * API may have. <br />
 * Feeding the encoder with a surface is not tested here. Some bugs you may have
 * encountered:<br />
 * <ul>
 * <li>U and V panes reversed</li>
 * <li>Some padding is needed after the Y pane</li>
 * <li>stride!=width or slice-height!=height</li>
 * </ul>
 */
@SuppressLint("NewApi")
public class EncoderDebugger {

    public final static String TAG = "EncoderDebugger";

    /**
     * Prefix that will be used for all shared preferences saved by
     * libstreaming.
     */
    private static final String PREF_PREFIX = "libstreaming-";

    /**
     * If this is set to false the test will be run only once and the result
     * will be saved in the shared preferences.
     */
    private static final boolean DEBUG = false;

    /**
     * Set this to true to see more logs.
     */
    private static final boolean VERBOSE = false;

    /**
     * Will be incremented every time this test is modified.
     */
    private static final int VERSION = 3;

    /**
     * Bitrate that will be used with the encoder.
     */
    private final static int BITRATE = 1000000;

    /**
     * Framerate that will be used to test the encoder.
     */
    private final static int FRAMERATE = 20;

    private final static String MIME_TYPE = "video/avc";

    private final static int NB_DECODED = 34;
    private final static int NB_ENCODED = 50;

    private int mEncoderColorFormat;
    private String mEncoderName, mErrorLog;
    private MediaCodec mEncoder;
    private int mWidth, mHeight, mSize;
    private byte[] mSPS, mPPS;

    private byte[] mData, mInitialImage;
    private NV21Convertor mNV21;
    private SharedPreferences mPreferences;
    private byte[][] mVideo, mDecodedVideo;
    private String mB64PPS, mB64SPS;

    public synchronized static void asyncDebug(final Context context,
                                               final int width, final int height) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    SharedPreferences prefs = PreferenceManager
                            .getDefaultSharedPreferences(context);
                    debug(prefs, width, height);
                } catch (Exception e) {
                }
            }
        }).start();
    }

    public synchronized static EncoderDebugger debug(Context context, int width, int height) {
        SharedPreferences prefs = PreferenceManager
                .getDefaultSharedPreferences(context);
        return debug(prefs, width, height);
    }

    public synchronized static EncoderDebugger debug(SharedPreferences prefs, int width, int height) {
        EncoderDebugger debugger = new EncoderDebugger(prefs, width, height);
        debugger.debug();
        return debugger;
    }

    public String getB64PPS() {
        return mB64PPS;
    }

    public String getB64SPS() {
        return mB64SPS;
    }

    public String getEncoderName() {
        return mEncoderName;
    }

    public int getEncoderColorFormat() {
        return mEncoderColorFormat;
    }

    /**
     * This {@link NV21Convertor} will do the necessary work to feed properly
     * the encoder.
     */
    public NV21Convertor getNV21Convertor() {
        return mNV21;
    }

    /**
     * A log of all the errors that occured during the test.
     */
    public String getErrorLog() {
        return mErrorLog;
    }

    private EncoderDebugger(SharedPreferences prefs, int width, int height) {
        mPreferences = prefs;
        mWidth = width;
        mHeight = height;
        mSize = width * height;
        reset();
    }

    private void reset() {
        mNV21 = new NV21Convertor();
        mVideo = new byte[NB_ENCODED][];
        mDecodedVideo = new byte[NB_DECODED][];
        mErrorLog = "";
        mPPS = null;
        mSPS = null;
    }

    private void debug() {

        // If testing the phone again is not needed,
        // we just restore the result from the shared preferences
        if (!checkTestNeeded()) {
            String resolution = mWidth + "x" + mHeight + "-";

            boolean success = mPreferences.getBoolean(PREF_PREFIX + resolution
                    + "success", false);
            if (!success) {
                throw new RuntimeException(
                        "Phone not supported with this resolution (" + mWidth
                                + "x" + mHeight + ")");
            }

            mNV21.setSize(mWidth, mHeight);
            mNV21.setSliceHeigth(mPreferences.getInt(PREF_PREFIX + resolution
                    + "sliceHeight", 0));
            mNV21.setStride(mPreferences.getInt(PREF_PREFIX + resolution
                    + "stride", 0));
            mNV21.setYPadding(mPreferences.getInt(PREF_PREFIX + resolution
                    + "padding", 0));
            mNV21.setPlanar(mPreferences.getBoolean(PREF_PREFIX + resolution
                    + "planar", false));
            mNV21.setColorPanesReversed(mPreferences.getBoolean(PREF_PREFIX
                    + resolution + "reversed", false));
            mEncoderName = mPreferences.getString(PREF_PREFIX + resolution
                    + "encoderName", "");
            mEncoderColorFormat = mPreferences.getInt(PREF_PREFIX + resolution
                    + "colorFormat", 0);
            mB64PPS = mPreferences.getString(PREF_PREFIX + resolution + "pps",
                    "");
            mB64SPS = mPreferences.getString(PREF_PREFIX + resolution + "sps",
                    "");

            return;
        }

        if (VERBOSE)
            Log.d(TAG, ">>>> Testing the phone for resolution " + mWidth + "x"
                    + mHeight);

        // Builds a list of available encoders and decoders we may be able to
        // use
        // because they support some nice color formats
        Codec[] encoders = CodecManager.findEncodersForMimeType(MIME_TYPE);
        Codec[] decoders = CodecManager.findDecodersForMimeType(MIME_TYPE);

        int count = 0, n = 1;
        for (int i = 0; i < encoders.length; i++) {
            count += encoders[i].formats.length;
        }

        // Tries available encoders
        for (int i = 0; i < encoders.length; i++) {
            for (int j = 0; j < encoders[i].formats.length; j++) {
                reset();

                mEncoderName = encoders[i].name;
                mEncoderColorFormat = encoders[i].formats[j];

                if (VERBOSE)
                    Log.v(TAG, ">> Test " + (n++) + "/" + count + ": "
                            + mEncoderName + " with color format "
                            + mEncoderColorFormat + " at " + mWidth + "x"
                            + mHeight);

                // Converts from NV21 to YUV420 with the specified parameters
                mNV21.setSize(mWidth, mHeight);
                mNV21.setSliceHeigth(mHeight);
                mNV21.setStride(mWidth);
                mNV21.setYPadding(0);
                mNV21.setEncoderColorFormat(mEncoderColorFormat);

                // /!\ NV21Convertor can directly modify the input
                createTestImage();
                mData = mNV21.convert(mInitialImage);

                try {

                    // Starts the encoder
                    configureEncoder();
                    searchSPSandPPS();

                    saveTestResult(true);
                    Log.v(TAG, "The encoder " + mEncoderName
                            + " is usable with resolution " + mWidth + "x"
                            + mHeight);
                    return;

                } catch (Exception e) {
                    StringWriter sw = new StringWriter();
                    PrintWriter pw = new PrintWriter(sw);
                    e.printStackTrace(pw);
                    String stack = sw.toString();
                    String str = "Encoder " + mEncoderName
                            + " cannot be used with color format "
                            + mEncoderColorFormat;
                    if (VERBOSE)
                        Log.e(TAG, str, e);
                    mErrorLog += str + "\n" + stack;
                    e.printStackTrace();
                } finally {
                    releaseEncoder();
                }

            }
        }

        saveTestResult(false);
        Log.e(TAG, "No usable encoder were found on the phone for resolution "
                + mWidth + "x" + mHeight);
        throw new RuntimeException(
                "No usable encoder were found on the phone for resolution "
                        + mWidth + "x" + mHeight);

    }


    private boolean checkTestNeeded() {
        String resolution = mWidth + "x" + mHeight + "-";

        // Forces the test
        if (DEBUG || mPreferences == null)
            return true;

        // If the sdk has changed on the phone, or the version of the test
        // it has to be run again
        if (mPreferences.contains(PREF_PREFIX + resolution + "lastSdk")) {
            int lastSdk = mPreferences.getInt(PREF_PREFIX + resolution
                    + "lastSdk", 0);
            int lastVersion = mPreferences.getInt(PREF_PREFIX + resolution
                    + "lastVersion", 0);
            if (Build.VERSION.SDK_INT > lastSdk || VERSION > lastVersion) {
                return true;
            }
        } else {
            return true;
        }
        return false;
    }

    /**
     * Saves the result of the test in the shared preferences, we will run it
     * again only if the SDK has changed on the phone, or if this test has been
     * modified.
     */
    private void saveTestResult(boolean success) {
        String resolution = mWidth + "x" + mHeight + "-";
        Editor editor = mPreferences.edit();

        editor.putBoolean(PREF_PREFIX + resolution + "success", success);

        if (success) {
            editor.putInt(PREF_PREFIX + resolution + "lastSdk",
                    Build.VERSION.SDK_INT);
            editor.putInt(PREF_PREFIX + resolution + "lastVersion", VERSION);
            editor.putInt(PREF_PREFIX + resolution + "sliceHeight",
                    mNV21.getSliceHeigth());
            editor.putInt(PREF_PREFIX + resolution + "stride",
                    mNV21.getStride());
            editor.putInt(PREF_PREFIX + resolution + "padding",
                    mNV21.getYPadding());
            editor.putBoolean(PREF_PREFIX + resolution + "planar",
                    mNV21.getPlanar());
            editor.putBoolean(PREF_PREFIX + resolution + "reversed",
                    mNV21.getUVPanesReversed());
            editor.putString(PREF_PREFIX + resolution + "encoderName",
                    mEncoderName);
            editor.putInt(PREF_PREFIX + resolution + "colorFormat",
                    mEncoderColorFormat);
            editor.putString(PREF_PREFIX + resolution + "encoderName",
                    mEncoderName);
            editor.putString(PREF_PREFIX + resolution + "pps", mB64PPS);
            editor.putString(PREF_PREFIX + resolution + "sps", mB64SPS);
        }

        editor.commit();
    }

    /**
     * Creates the test image that will be used to feed the encoder.
     */
    private void createTestImage() {
        mInitialImage = new byte[3 * mSize / 2];
        for (int i = 0; i < mSize; i++) {
            mInitialImage[i] = (byte) (40 + i % 199);
        }
        for (int i = mSize; i < 3 * mSize / 2; i += 2) {
            mInitialImage[i] = (byte) (40 + i % 200);
            mInitialImage[i + 1] = (byte) (40 + (i + 99) % 200);
        }

    }

    /**
     * Converts the image obtained from the decoder to NV21.
     */

    /**
     * Instantiates and starts the encoder.
     *
     * @throws IOException
     */
    private void configureEncoder() throws IOException {
        mEncoder = MediaCodec.createByCodecName(mEncoderName);
        MediaFormat mediaFormat = MediaFormat.createVideoFormat(MIME_TYPE,
                mWidth, mHeight);
        mediaFormat.setInteger(MediaFormat.KEY_BIT_RATE, BITRATE);
        mediaFormat.setInteger(MediaFormat.KEY_FRAME_RATE, FRAMERATE);
        mediaFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT,
                mEncoderColorFormat);
        mediaFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 1);
        mEncoder.configure(mediaFormat, null, null,
                MediaCodec.CONFIGURE_FLAG_ENCODE);
        mEncoder.start();
    }

    private void releaseEncoder() {
        if (mEncoder != null) {
            try {
                mEncoder.stop();
            } catch (Exception ignore) {
            }
            try {
                mEncoder.release();
            } catch (Exception ignore) {
            }
        }

    }

    /**
     * Tries to obtain the SPS and the PPS for the encoder.
     */
    private long searchSPSandPPS() {
        long elapsed = 0, now = timestamp();
        ByteBuffer[] inputBuffers = mEncoder.getInputBuffers();
        ByteBuffer[] outputBuffers = mEncoder.getOutputBuffers();
        BufferInfo info = new BufferInfo();
        byte[] csd = new byte[128];
        int len = 0, p = 4, q = 4;

        while (elapsed < 3000000 && (mSPS == null || mPPS == null)) {

            // Some encoders won't give us the SPS and PPS unless they receive
            // something to encode first...
            int bufferIndex = mEncoder.dequeueInputBuffer(1000000 / FRAMERATE);
            if (bufferIndex >= 0) {
                check(inputBuffers[bufferIndex].capacity() >= mData.length,
                        "The input buffer is not big enough.");
                inputBuffers[bufferIndex].clear();
                inputBuffers[bufferIndex].put(mData, 0, mData.length);
                mEncoder.queueInputBuffer(bufferIndex, 0, mData.length,
                        timestamp(), 0);
            } else {
                if (VERBOSE)
                    Log.e(TAG, "No buffer available !");
            }

            // We are looking for the SPS and the PPS here. As always, Android
            // is very inconsistent, I have observed that some
            // encoders will give those parameters through the MediaFormat
            // object (that is the normal behaviour).
            // But some other will not, in that case we try to find a NAL unit
            // of type 7 or 8 in the byte stream outputed by the encoder...

            int index = mEncoder.dequeueOutputBuffer(info, 1000000 / FRAMERATE);

            if (index == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {

                // The PPS and PPS shoud be there
                MediaFormat format = mEncoder.getOutputFormat();
                ByteBuffer spsb = format.getByteBuffer("csd-0");
                ByteBuffer ppsb = format.getByteBuffer("csd-1");
                mSPS = new byte[spsb.capacity() - 4];
                spsb.position(4);
                spsb.get(mSPS, 0, mSPS.length);
                mPPS = new byte[ppsb.capacity() - 4];
                ppsb.position(4);
                ppsb.get(mPPS, 0, mPPS.length);
                break;

            } else if (index == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
                outputBuffers = mEncoder.getOutputBuffers();
            } else if (index >= 0) {

                len = info.size;
                if (len < 128) {
                    outputBuffers[index].get(csd, 0, len);
                    if (len > 0 && csd[0] == 0 && csd[1] == 0 && csd[2] == 0
                            && csd[3] == 1) {
                        // Parses the SPS and PPS, they could be in two
                        // different packets and in a different order
                        // depending on the phone so we don't make any
                        // assumption about that
                        while (p < len) {
                            while (!(csd[p + 0] == 0 && csd[p + 1] == 0
                                    && csd[p + 2] == 0 && csd[p + 3] == 1)
                                    && p + 3 < len)
                                p++;
                            if (p + 3 >= len)
                                p = len;
                            if ((csd[q] & 0x1F) == 7) {
                                mSPS = new byte[p - q];
                                System.arraycopy(csd, q, mSPS, 0, p - q);
                            } else {
                                mPPS = new byte[p - q];
                                System.arraycopy(csd, q, mPPS, 0, p - q);
                            }
                            p += 4;
                            q = p;
                        }
                    }
                }
                mEncoder.releaseOutputBuffer(index, false);
            }

            elapsed = timestamp() - now;
        }

        check(mPPS != null & mSPS != null, "Could not determine the SPS & PPS.");
        mB64PPS = Base64.encodeToString(mPPS, 0, mPPS.length, Base64.NO_WRAP);
        mB64SPS = Base64.encodeToString(mSPS, 0, mSPS.length, Base64.NO_WRAP);

        return elapsed;
    }

    static int getXPS(byte[] data, int offset, int length, byte[] dataOut,
                      int[] outLen, int type) {
        int i;
        int pos0;
        int pos1;
        pos0 = -1;
        for (i = offset; i < length - 4; i++) {
            if ((0 == data[i]) && (0 == data[i + 1]) && (1 == data[i + 2])
                    && (type == (0x0F & data[i + 3]))) {
                pos0 = i;
                break;
            }
        }
        if (-1 == pos0) {
            return -1;
        }
        pos1 = -1;
        for (i = pos0 + 4; i < length - 4; i++) {
            if ((0 == data[i]) && (0 == data[i + 1]) && (0 == data[i + 2])) {
                pos1 = i;
                break;
            }
        }
        if (-1 == pos1) {
            return -2;
        }
        if (pos1 - pos0 + 1 > outLen[0]) {
            return -3; // 输入缓冲区太小
        }
        dataOut[0] = 0;
        System.arraycopy(data, pos0, dataOut, 1, pos1 - pos0);
        // memcpy(pXPS+1, pES+pos0, pos1-pos0);
        // *pMaxXPSLen = pos1-pos0+1;
        outLen[0] = pos1 - pos0 + 1;
        return 0;
    }


    private void check(boolean cond, String message) {
        if (!cond) {
            if (VERBOSE)
                Log.e(TAG, message);
            throw new IllegalStateException(message);
        }
    }

    private long timestamp() {
        return System.nanoTime() / 1000;
    }

    @Override
    public String toString() {
        return "EncoderDebugger [mEncoderColorFormat=" + mEncoderColorFormat
                + ", mEncoderName=" + mEncoderName + ", mErrorLog=" + mErrorLog + ", mEncoder="
                + mEncoder + ", mWidth=" + mWidth
                + ", mHeight=" + mHeight + ", mSize=" + mSize + ", mSPS="
                + Arrays.toString(mSPS) + ", mPPS=" + Arrays.toString(mPPS)
                + ", mData=" + Arrays.toString(mData) + ", mInitialImage="
                + Arrays.toString(mInitialImage) + ", mNV21=" + mNV21 + ", mPreferences="
                + mPreferences + ", mVideo=" + Arrays.toString(mVideo)
                + ", mDecodedVideo=" + Arrays.toString(mDecodedVideo)
                + ", mB64PPS=" + mB64PPS + ", mB64SPS=" + mB64SPS
                + "]";
    }


}
