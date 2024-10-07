package com.committeeofzero.impacto;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Arrays;

import android.os.Bundle;
import android.os.Environment;
import android.content.res.AssetManager;
import android.util.Log;

import org.libsdl.app.SDLActivity;

public class ImpactoActivity extends SDLActivity {

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
        File externalFilesDir = getExternalFilesDir(null);
        copyAssetFolder("games", externalFilesDir.getAbsolutePath() + "/" + "games");
        copyAssetFolder("profiles", externalFilesDir.getAbsolutePath() + "/" + "profiles");
        copyAssetFolder("shaders", getFilesDir().getAbsolutePath() + "/" + "shaders");
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
