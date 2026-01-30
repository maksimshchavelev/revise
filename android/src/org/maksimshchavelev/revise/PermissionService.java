package org.maksimshchavelev.revise;

import android.app.Activity;
import android.app.AlarmManager;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.BroadcastReceiver;
import android.content.pm.PackageManager;
import android.os.Build;
import android.provider.Settings;

import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * Permission helper that provides synchronous (blocking) requestPermission.
 *
 * WARNING: requestPermission blocks the calling thread until result or timeout (30 s).
 * DO NOT call it from UI thread.
 */
public class PermissionService {
    private final Activity activity;
    private static final int REQUEST_CODE = 1001;
    private static final String ACTION_PERMISSION_RESULT = "org.maksimshchavelev.revise.ACTION_PERMISSION_RESULT";

    public PermissionService(Activity activity) {
        this.activity = activity;
    }

    /**
     * Check permission quickly.
     */
    public boolean hasPermission(String permission) {
        if (permission == null) return false;

        // Special-case exact alarms
        if ("android.permission.SCHEDULE_EXACT_ALARM".equals(permission)) {
            AlarmManager am = (AlarmManager) activity.getSystemService(Context.ALARM_SERVICE);
            if (am == null) return false;
            return am.canScheduleExactAlarms();
        }

        // Normal runtime permission
        return ContextCompat.checkSelfPermission(activity, permission) == PackageManager.PERMISSION_GRANTED;
    }

    /**
     * Request permission synchronously. Blocks until user answers or timeout (30s).
     *
     * Note: must be called from non-UI thread. Internally launches a transient Activity
     * (PermissionRequestActivity) that requests permission and broadcasts the result.
     *
     * Supported permissions:
     *  - normal runtime permissions (POST_NOTIFICATIONS etc.)
     *  - android.permission.SCHEDULE_EXACT_ALARM (opens system settings and polls canScheduleExactAlarms)
     *
     * @param permission permission string to request
     */
    public void requestPermission(String permission) {
        if (permission == null) return;

        // Special-case exact alarm permission (needs settings UI)
        if ("android.permission.SCHEDULE_EXACT_ALARM".equals(permission)) {
            AlarmManager am = (AlarmManager) activity.getSystemService(Context.ALARM_SERVICE);
            if (am == null) return;

            if (am.canScheduleExactAlarms()) {
                // already allowed
                return;
            }

            // Build intent to system UI where user can grant scheduling exact alarms
            Intent intent = new Intent();
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                intent.setAction(Settings.ACTION_REQUEST_SCHEDULE_EXACT_ALARM);
                // optional: show only our package
                intent.setData(android.net.Uri.parse("package:" + activity.getPackageName()));
            } else {
                // older devices — no special flow, nothing to request
                return;
            }

            // Start settings activity (non-blocking) and then poll for result
            activity.startActivity(intent);

            // Poll for up to 30s checking canScheduleExactAlarms()
            final long deadline = System.currentTimeMillis() + TimeUnit.SECONDS.toMillis(30);
            while (System.currentTimeMillis() < deadline) {
                if (am.canScheduleExactAlarms()) return;
                try { Thread.sleep(500); } catch (InterruptedException e) { Thread.currentThread().interrupt(); return; }
            }
            // timeout -> return (permission not granted)
            return;
        }

        // Normal runtime permission flow: use PermissionRequestActivity + BroadcastReceiver
        final CountDownLatch latch = new CountDownLatch(1);
        final AtomicBoolean granted = new AtomicBoolean(false);

        // Setup receiver on application context to get result from PermissionRequestActivity
        BroadcastReceiver receiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context ctx, Intent intent) {
                if (intent == null) return;
                boolean ok = intent.getBooleanExtra("granted", false);
                granted.set(ok);
                try {
                    // Unregister safely
                    ctx.unregisterReceiver(this);
                } catch (IllegalArgumentException ignored) {}
                latch.countDown();
            }
        };

        IntentFilter filter = new IntentFilter(ACTION_PERMISSION_RESULT);
        Context appCtx = activity.getApplicationContext();
        appCtx.registerReceiver(receiver, filter);

        // Launch PermissionRequestActivity (transparent) to request the permission.
        Intent req = new Intent(activity, PermissionRequestActivity.class);
        req.putExtra("permission", permission);
        req.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        activity.startActivity(req);

        // Wait up to 30 seconds for result
        try {
            latch.await(30, TimeUnit.SECONDS);
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        } finally {
            // ensure receiver cleanup in case of timeout
            try { appCtx.unregisterReceiver(receiver); } catch (IllegalArgumentException ignored) {}
        }

        // granted.get() contains result (but we don't return value because signature is void)
        return;
    }
}
