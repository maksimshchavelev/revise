package org.maksimshchavelev.revise;

import android.app.Activity;
import android.content.pm.PackageManager;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

public class PermissionService {
    private final Activity activity;
    private static final int REQUEST_CODE = 1001;

    public PermissionService(Activity activity) {
        this.activity = activity;
    }

    public boolean hasPermission(String permission) {
        return ContextCompat.checkSelfPermission(activity, permission) == PackageManager.PERMISSION_GRANTED;
    }

    public void requestPermission(String permission) {
        ActivityCompat.requestPermissions(activity, new String[]{permission}, REQUEST_CODE);
    }
}
