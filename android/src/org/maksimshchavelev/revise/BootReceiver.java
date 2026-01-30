package org.maksimshchavelev.revise;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

import java.util.Set;

/**
 * After device reboot, reschedule stored alarms.
 */
public class BootReceiver extends BroadcastReceiver {
    @Override
    public void onReceive(Context context, Intent intent) {
        if (intent == null) return;
        if (Intent.ACTION_BOOT_COMPLETED.equals(intent.getAction()) ||
            Intent.ACTION_LOCKED_BOOT_COMPLETED.equals(intent.getAction())) {

            NotificationService svc = new NotificationService(context);
            // Retrieve stored ids — but we don't have stored timestamps in current sample.
            // For production you'd store also epochMillis per id and restore alarms here.
            // Here we simply clear stale ids to prevent leaks.
            svc.clearAllScheduledNotifications();
            // TODO: to re-schedule after reboot, persist schedule times per id and restore here.
        }
    }
}

