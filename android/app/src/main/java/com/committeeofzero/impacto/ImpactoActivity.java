package com.committeeofzero.impacto;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.content.SharedPreferences;

import org.libsdl.app.SDLActivity;

public class ImpactoActivity extends SDLActivity {
    SharedPreferences prefs = null;

    /**
     * This method is called by SDL before loading the native shared libraries.
     * It can be overridden to provide names of shared libraries to be loaded.
     * The default implementation returns the defaults. It never returns null.
     * An array returned by a new implementation must at least contain "SDL2".
     * Also keep in mind that the order the libraries are loaded may matter.
     *
     * @return names of shared libraries to be loaded (e.g. "SDL2", "main").
     */
    @Override
    protected String[] getLibraries() {
        return new String[]{
            "impacto"
        };
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        prefs = this.getPreferences(Context.MODE_PRIVATE);
        File externalFilesDir = getExternalFilesDir(null);
        File resetFile = new File(externalFilesDir, ".reset");
        boolean reset = prefs.getBoolean("firstRun", true);
        if (resetFile.exists()) {
            resetFile.delete();
            reset = true;
        }
        if (reset) {            
            copyAssetFolder("games", externalFilesDir.getAbsolutePath() + "/" + "games");
            copyAssetFolder("profiles", externalFilesDir.getAbsolutePath() + "/" + "profiles");
            copyAssetFolder("shaders", getFilesDir().getAbsolutePath() + "/" + "shaders");
        }
        super.onCreate(savedInstanceState);
    }

    public boolean copyAssetFolder(String srcName, String dstName) {
        try {
            boolean result = true;
            String[] fileList = getAssets().list(srcName);
            if (fileList == null) {
                return false;
            }

            if (fileList.length == 0) {
                result = copyAssetFile(srcName, dstName);
            } else {
                File file = new File(dstName);
                result = file.mkdirs();
                for (String filename : fileList) {
                    result &= copyAssetFolder(srcName + File.separator + filename, dstName + File.separator + filename);
                }
            }
            return result;
        } catch (IOException e) {
            Log.e(null, "Failed to copy folder \"" + srcName + ", error: \"" + e.getMessage() + "\"\n.");
            return false;
        }
    }

    public boolean copyAssetFile(String srcName, String dstName) {
        try {
            InputStream in = getAssets().open(srcName);
            File outFile = new File(dstName);
            OutputStream out = new FileOutputStream(outFile);
            byte[] buffer = new byte[1024];
            int read;
            while ((read = in.read(buffer)) != -1) {
                out.write(buffer, 0, read);
            }
            in.close();
            out.close();
            return true;
        } catch (IOException e) {
            Log.e(null, "Failed to copy file \"" + srcName + ", error: \"" + e.getMessage() + "\"\n.");
            return false;
        }
    }

}
