package com.committeeofzero.impacto;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Objects;
import java.util.function.Function;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.ParcelFileDescriptor;
import android.provider.DocumentsContract;
import android.provider.Settings;
import android.util.Log;
import android.content.SharedPreferences;
import android.widget.Toast;

import androidx.documentfile.provider.DocumentFile;

import org.libsdl.app.SDLActivity;

public class ImpactoActivity extends SDLActivity {
    public static final int RequestFolderCode = 1000;
    public static final String SharedPrefsUpdateTimeKey = "lastUpdateTime";
    public static final String SharedPrefsChosenDirKey = "pickedDirectory";

    private ParcelFileDescriptor cachedFd;

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
        return new String[]{
                "impacto"
        };
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        File externalFilesDir = getExternalFilesDir(null);

        if (isFirstInstall() || isUpdated()) {
            copyAssetFolder("resources", getFilesDir().getAbsolutePath() + "/" + "resources");
            copyAssetFolder("profiles", externalFilesDir.getAbsolutePath() + "/" + "profiles");

            copyAssetFile("gamedefinitions.lua", externalFilesDir.getAbsolutePath() + "/" + "gamedefinitions.lua");
        }
        updateBasePaths();
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
            long prevUpdateTime = prefs.getLong(SharedPrefsUpdateTimeKey, firstInstallTime);

            SharedPreferences.Editor editor = prefs.edit();
            editor.putLong(SharedPrefsUpdateTimeKey, lastUpdateTime);
            editor.apply();
            return prevUpdateTime != lastUpdateTime || prevUpdateTime == firstInstallTime;
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
            return false;
        }
    }

    public void updateBasePaths() {
        File externalFilesDir = getExternalFilesDir(null);
        String chosenDir = getChosenDirectory();
        if (externalFilesDir == null) return;
        String basepathsContents = String.format("""
                        root.BasePaths = {
                                RootInstallDir = "%1$s",
                                RootGamedataDir = "%2$s/gamedata",
                                RootProfilesDir = "%3$s/profiles",
                                RootPatchesDir = "%2$s/patches",
                                RootSavesDir = "%2$s/saves",
                            };
                        """,
                getFilesDir(),
                chosenDir,
                externalFilesDir.getAbsolutePath()
        );
        File basepaths = new File(externalFilesDir.getAbsolutePath(), "basepaths.lua");
        try (FileOutputStream fos = new FileOutputStream(basepaths)) {
            fos.write(basepathsContents.getBytes());
        } catch (IOException e) {
            Log.e("IO", "Error creating or writing basepaths", e);
        }

        if (chosenDir != null) {
            SharedPreferences prefs = this.getSharedPreferences("impacto", Context.MODE_PRIVATE);
            String uriString = prefs.getString(SharedPrefsChosenDirKey, null);
            if(uriString == null) return;
            DocumentFile pickedDir = DocumentFile.fromTreeUri(getApplicationContext(), Uri.parse(uriString));
            if(pickedDir == null) return;
            if(pickedDir.findFile("gamedata") == null) pickedDir.createDirectory("gamedata");
            if(pickedDir.findFile("patches") == null) pickedDir.createDirectory("patches");
            if(pickedDir.findFile("saves") == null) pickedDir.createDirectory("saves");
        }
    }

    public void chooseDirectory() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R && !Environment.isExternalStorageManager()) {
            Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION)
                    .setData(Uri.parse("package:" + getPackageName()));
            startActivity(intent);
            return;
        }

        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT_TREE);
        intent.putExtra(Intent.EXTRA_LOCAL_ONLY, true);
        intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION |
                Intent.FLAG_GRANT_WRITE_URI_PERMISSION |
                Intent.FLAG_GRANT_PERSISTABLE_URI_PERMISSION);
        startActivityForResult(intent, RequestFolderCode);
    }

    // https://stackoverflow.com/a/58304368/27686485
    public String getChosenDirectory() {
        String result = getExternalFilesDir(null).toString();
        Function<ParcelFileDescriptor, String> fdToPath = parcelFileDescriptor -> {
            try {
                int fd = parcelFileDescriptor.getFd();
                Path symlink = Paths.get("/proc/self/fd/" + fd);
                String mntPath = Files.readSymbolicLink(symlink).toAbsolutePath().toString();
                return mntPath.replaceAll("/mnt/user/(\\d)/", "/storage/");
            } catch (IOException e) {
                showToast("Error accessing directory: " + e.getMessage(), Toast.LENGTH_LONG, -1, 0, 0);
                return null;
            }
        };

        if (cachedFd != null) return fdToPath.apply(cachedFd);

        SharedPreferences prefs = this.getSharedPreferences("impacto", Context.MODE_PRIVATE);
        String uriString = prefs.getString(SharedPrefsChosenDirKey, null);
        if (uriString == null) return result;
        DocumentFile tree = null;
        try {
            Uri uri = Uri.parse(uriString);
            tree = DocumentFile.fromTreeUri(getApplicationContext(), uri);
        } catch (IllegalArgumentException e) {
            prefs.edit().remove(SharedPrefsChosenDirKey).apply();
        }
        if (tree == null) return result;

        try {
            ParcelFileDescriptor parcelFileDescriptor = getContentResolver().openFileDescriptor(tree.getUri(), "r");
            String resolvedPath = fdToPath.apply(parcelFileDescriptor);
            if(resolvedPath != null) {
                result = resolvedPath;
                cachedFd = parcelFileDescriptor;
            }
        } catch (FileNotFoundException e) {
            Log.e("IO", "Error accessing directory", e);
        }
        return result;
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode,
                                 Intent resultData) {
        if (requestCode == RequestFolderCode && resultCode == Activity.RESULT_OK && resultData != null) {
            Uri uri = resultData.getData();
            if (uri == null) return;
            SharedPreferences prefs = this.getSharedPreferences("impacto", Context.MODE_PRIVATE);
            String oldUriString = prefs.getString(SharedPrefsChosenDirKey, null);

            SharedPreferences.Editor editor = prefs.edit();
            editor.putString(SharedPrefsChosenDirKey, uri.toString());
            editor.apply();

            try {
                getContentResolver().takePersistableUriPermission(uri, Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
            } catch (SecurityException e) {
                Log.e("IO", "Failed to persist URI permission", e);
                showToast("Could not access that folder: " + e.getMessage(), Toast.LENGTH_LONG, -1, 0, 0);
                return;
            }

            // Only revoke the old grant if it's a different URI than the one we just took
            if (oldUriString != null && !oldUriString.equals(uri.toString())) {
                revokeUriPermission(Uri.parse(oldUriString), Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
            }
            cachedFd = null;
            updateBasePaths();
        }
        super.onActivityResult(requestCode, resultCode, resultData);
    }
}
