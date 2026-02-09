package org.maksimshchavelev.revise;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

import androidx.annotation.NonNull;

/**
 * Receives alarms from AlarmManager and shows notification.
 */
public class AlarmReceiver extends BroadcastReceiver {
    @Override
    public void onReceive(Context context, Intent intent) {
        if (intent == null) return;

        String action = intent.getAction();
        // match the action we used when scheduling
        if ("org.maksimshchavelev.revise.ACTION_ALARM".equals(action) || intent.hasExtra("notification_text")) {
            String text = intent.getStringExtra("notification_text");
            int id = intent.getIntExtra("notification_id", (int) (System.currentTimeMillis() & 0x7FFFFFFF));

            // Use NotificationService to show now — create instance with context
            NotificationService svc = new NotificationService(context);
            svc.showNotificationWithId(id, text); // <- публичный wrapper
        }
    }
}
