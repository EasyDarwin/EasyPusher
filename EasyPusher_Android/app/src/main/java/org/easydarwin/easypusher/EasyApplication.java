package org.easydarwin.easypusher;

import android.app.Application;
import android.hardware.Camera;
import android.widget.ArrayAdapter;

import org.easydarwin.util.Util;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;

/**
 * Created by Helong on 16/4/16-12:54.
 */
public class EasyApplication extends Application {

    @Override
    public void onCreate() {
        super.onCreate();

        if(Util.getSupportResolution(this).size()==0){
            StringBuilder stringBuilder=new StringBuilder();
            Camera camera=Camera.open();
            List<Camera.Size> supportedPreviewSizes = camera.getParameters().getSupportedPreviewSizes();
            for (Camera.Size str : supportedPreviewSizes){
                stringBuilder.append(str.width + "x" + str.height).append(";");
            }
            Util.saveSupportResolution(this,stringBuilder.toString());
            camera.release();
        }


    }

}
