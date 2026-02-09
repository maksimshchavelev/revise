package org.maksimshchavelev.revise;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;

/**
 * Temporary Activity used to request a runtime permission and broadcast the result.
 * This Activity is transparent (no UI) and closes immediately when result arrives.
 */
public class PermissionRequestActivity extends Activity {
    private static final int REQ = 2002;
    private String mPermission;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Intent it = getIntent();
        if (it == null) {
            finish();
            return;
        }

        mPermission = it.getStringExtra("permission");
        if (mPermission == null) {
            finish();
            return;
        }

        // Request permission using ActivityCompat — callback comes to this Activity
        ActivityCompat.requestPermissions(this, new String[]{mPermission}, REQ);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        boolean granted = false;
        if (requestCode == REQ && grantResults != null && grantResults.length > 0) {
            granted = grantResults[0] == android.content.pm.PackageManager.PERMISSION_GRANTED;
        }

        // Broadcast result so the original caller (PermissionService) can unblock
        Intent r = new Intent("org.maksimshchavelev.revise.ACTION_PERMISSION_RESULT");
        r.putExtra("granted", granted);
        getApplicationContext().sendBroadcast(r);

        // finish this transient Activity
        finish();
    }
}

