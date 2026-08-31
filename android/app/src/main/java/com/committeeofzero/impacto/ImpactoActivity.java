package com.committeeofzero.impacto;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.content.SharedPreferences;

import org.libsdl.app.SDLActivity;

public class ImpactoActivity extends SDLActivity {
    /**
     * This method is called by SDL before loading the native shared libraries.
     * It can be overridden to provide names of shared libraries to be loaded.
     * The default implementation returns the defaults. It never returns null.
     * An array returned by a new implementation must at least contain "SDL3".
     * Also keep in mind that the order the libraries are loaded may matter.
     *
     * @return names of shared libraries to be loaded (e.g. "SDL3", "main").
     */
    @Override
    protected String[] getLibraries() {
        return new String[] {
                "impacto"
        };
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        File externalFilesDir = getExternalFilesDir(null);

        // Todo: User customizable paths using the startup activity screen
        // Will need to use file descriptor technique outlined here:
        // https://stackoverflow.com/a/58304368/27686485
        // then we get cannonical path and write it.
        String basepathsContents = String.format("""
                root.BasePaths = {
                    RootInstallDir = "%s",
                    RootGamedataDir = "%s/gamedata",
                    RootProfilesDir = "%s/profiles",
                    RootPatchesDir = "%s/patches",
                    RootSavesDir = "%s/saves",
                };
                """, "./", externalFilesDir.getAbsolutePath(), externalFilesDir.getAbsolutePath(),
                externalFilesDir.getAbsolutePath(), externalFilesDir.getAbsolutePath());
        File basepaths = new File(externalFilesDir.getAbsolutePath(), "basepaths.lua");
        try (FileOutputStream fos = new FileOutputStream(basepaths)) {
            fos.write(basepathsContents.getBytes());
        } catch (IOException e) {
            Log.e("FileStatus", "Error creating or writing basepaths", e);
        }

        if(isFirstInstall() || isUpdated()) {
            copyAssetFolder("resources", getFilesDir().getAbsolutePath() + "/" + "resources");
            copyAssetFolder("profiles", externalFilesDir.getAbsolutePath() + "/" + "profiles");

            copyAssetFile("gamedefinitions.lua", externalFilesDir.getAbsolutePath() + "/" + "gamedefinitions.lua");
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

    // https://stackoverflow.com/a/34194960/27686485
    public boolean isFirstInstall() {
        try {
            long firstInstallTime = getPackageManager().getPackageInfo(getPackageName(), 0).firstInstallTime;
            long lastUpdateTime = getPackageManager().getPackageInfo(getPackageName(), 0).lastUpdateTime;
            return firstInstallTime == lastUpdateTime;
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
            return true;
        }
    }

    public boolean isUpdated() {
        try {
            SharedPreferences prefs = this.getSharedPreferences("impacto", Context.MODE_PRIVATE);

            long firstInstallTime = getPackageManager().getPackageInfo(getPackageName(), 0).firstInstallTime;


            long lastUpdateTime = getPackageManager().getPackageInfo(getPackageName(), 0).lastUpdateTime;
            long prevUpdateTime = prefs.getLong("lastUpdateTime", lastUpdateTime);

            SharedPreferences.Editor editor = prefs.edit();
            editor.putLong("lastUpdateTime", lastUpdateTime);
            editor.apply();
            return firstInstallTime != lastUpdateTime;
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
            return false;
        }
    }

}
