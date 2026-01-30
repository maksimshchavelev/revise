package org.maksimshchavelev.revise;

import android.app.Activity;
import android.app.AlarmManager;
import android.app.Application;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.provider.Settings;
import android.util.Log;
import android.os.Handler;
import android.os.Looper;

import androidx.core.content.ContextCompat;

import java.util.concurrent.CountDownLatch;

/**
 * Permission helper. Blocking requestPermission, safe to call from non-UI thread.
 *
 * Implementation details:
 * - For runtime permissions: start PermissionRequestActivity, then block until the **main activity**
 *   receives onResume (i.e. user returned from permission dialog). Then check permission with
 *   ContextCompat.checkSelfPermission(...) and return.
 * - For SCHEDULE_EXACT_ALARM: open settings (ACTION_REQUEST_SCHEDULE_EXACT_ALARM) with small delay,
 *   block until main activity onResume, then check AlarmManager.canScheduleExactAlarms().
 *
 * This provides strict blocking behavior (no timeouts) and avoids unreliable broadcasts.
 */
public class PermissionService {
    private final Activity activity;
    private static final String TAG = "PermissionService";
    // kept for compatibility if you want to use broadcast-based flow later
    private static final String ACTION_PERMISSION_RESULT = "org.maksimshchavelev.revise.ACTION_PERMISSION_RESULT";

    public PermissionService(Activity activity) {
        this.activity = activity;
    }

    /**
     * Quick synchronous check.
     */
    public boolean hasPermission(String permission) {
        if (permission == null) return false;

        if ("android.permission.SCHEDULE_EXACT_ALARM".equals(permission)) {
            AlarmManager am = (AlarmManager) activity.getSystemService(Context.ALARM_SERVICE);
            if (am == null) return false;
            return am.canScheduleExactAlarms();
        }

        return ContextCompat.checkSelfPermission(activity, permission) == PackageManager.PERMISSION_GRANTED;
    }

    /**
     * Blocking requestPermission. Safe to call from a worker thread (blocks that thread).
     * The method blocks until user finishes interaction and the app's main activity resumes,
     * then checks the permission state and returns.
     */
    public void requestPermission(final String permission) {
        if (permission == null) return;

        // ---------- SCHEDULE_EXACT_ALARM ----------
        if ("android.permission.SCHEDULE_EXACT_ALARM".equals(permission)) {
            AlarmManager am = (AlarmManager) activity.getSystemService(Context.ALARM_SERVICE);
            if (am == null) {
                Log.w(TAG, "AlarmManager == null");
                return;
            }
            if (am.canScheduleExactAlarms()) {
                Log.d(TAG, "SCHEDULE_EXACT_ALARM already allowed");
                return;
            }

            final CountDownLatch latch = new CountDownLatch(1);
            final Application app = activity.getApplication();

            final Application.ActivityLifecycleCallbacks callback = new Application.ActivityLifecycleCallbacks() {
                @Override public void onActivityCreated(Activity a, android.os.Bundle b) {}
                @Override public void onActivityStarted(Activity a) {}
                @Override public void onActivityResumed(Activity a) {
                    // When our main activity resumes after user returns from settings, unblock.
                    if (a == activity) {
                        try { app.unregisterActivityLifecycleCallbacks(this); } catch (Throwable ignored) {}
                        latch.countDown();
                    }
                }
                @Override public void onActivityPaused(Activity a) {}
                @Override public void onActivityStopped(Activity a) {}
                @Override public void onActivitySaveInstanceState(Activity a, android.os.Bundle b) {}
                @Override public void onActivityDestroyed(Activity a) {}
            };

            app.registerActivityLifecycleCallbacks(callback);

            // Start settings on UI thread with small delay to avoid race with previous dialogs
            Handler h = new Handler(Looper.getMainLooper());
            activity.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    try {
                        Intent intent = new Intent();
                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                            intent.setAction(Settings.ACTION_REQUEST_SCHEDULE_EXACT_ALARM);
                        } else {
                            // No action on older devices
                            try { app.unregisterActivityLifecycleCallbacks(callback); } catch (Throwable ignored) {}
                            latch.countDown();
                            return;
                        }
                        // small delay (300ms) to give system time to finish previous dialog transitions
                        h.postDelayed(new Runnable() {
                            @Override
                            public void run() {
                                try {
                                    activity.startActivity(intent);
                                } catch (Throwable t) {
                                    Log.w(TAG, "Failed to start exact alarm settings: " + t);
                                    try { app.unregisterActivityLifecycleCallbacks(callback); } catch (Throwable ignored) {}
                                    latch.countDown();
                                }
                            }
                        }, 300);
                    } catch (Throwable t) {
                        Log.w(TAG, "Failed to schedule exact alarm settings start: " + t);
                        try { app.unregisterActivityLifecycleCallbacks(callback); } catch (Throwable ignored) {}
                        latch.countDown();
                    }
                }
            });

            // BLOCK here until activity resume callback triggers (user returned)
            try {
                latch.await(); // <- blocking, no timeout (intentional)
            } catch (InterruptedException ie) {
                Thread.currentThread().interrupt();
            } finally {
                try { app.unregisterActivityLifecycleCallbacks(callback); } catch (Throwable ignored) {}
            }

            // After return, verify
            if (am.canScheduleExactAlarms()) {
                Log.d(TAG, "SCHEDULE_EXACT_ALARM granted after return");
            } else {
                Log.d(TAG, "SCHEDULE_EXACT_ALARM not granted after return");
            }
            return;
        }

        // ---------- Runtime permission (POST_NOTIFICATIONS etc.) ----------
        final CountDownLatch latch = new CountDownLatch(1);
        final Application app = activity.getApplication();

        final Application.ActivityLifecycleCallbacks callback = new Application.ActivityLifecycleCallbacks() {
            @Override public void onActivityCreated(Activity a, android.os.Bundle b) {}
            @Override public void onActivityStarted(Activity a) {}
            @Override public void onActivityResumed(Activity a) {
                // When main activity resumes after permission dialog, unblock.
                if (a == activity) {
                    try { app.unregisterActivityLifecycleCallbacks(this); } catch (Throwable ignored) {}
                    latch.countDown();
                }
            }
            @Override public void onActivityPaused(Activity a) {}
            @Override public void onActivityStopped(Activity a) {}
            @Override public void onActivitySaveInstanceState(Activity a, android.os.Bundle b) {}
            @Override public void onActivityDestroyed(Activity a) {}
        };

        app.registerActivityLifecycleCallbacks(callback);

        // Launch PermissionRequestActivity on UI thread
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                try {
                    Intent req = new Intent(activity, PermissionRequestActivity.class);
                    req.putExtra("permission", permission);
                    req.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                    activity.startActivity(req);
                } catch (Throwable t) {
                    Log.w(TAG, "Failed to start PermissionRequestActivity: " + t);
                    try { app.unregisterActivityLifecycleCallbacks(callback); } catch (Throwable ignored) {}
                    latch.countDown();
                }
            }
        });

        // BLOCK until main activity resumes (no timeout)
        try {
            latch.await(); // <- blocking, no timeout
        } catch (InterruptedException ie) {
            Thread.currentThread().interrupt();
        } finally {
            try { app.unregisterActivityLifecycleCallbacks(callback); } catch (Throwable ignored) {}
        }

        // Now check actual permission state and return
        boolean granted = (permission != null) && (ContextCompat.checkSelfPermission(activity, permission) == PackageManager.PERMISSION_GRANTED);
        Log.d(TAG, "requestPermission: " + permission + " result after return -> " + granted);
    }
}
