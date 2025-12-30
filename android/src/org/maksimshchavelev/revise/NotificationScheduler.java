package org.maksimshchavelev.revise;

import android.app.AlarmManager;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Build;

import java.util.HashSet;
import java.util.Set;

/**
 * Utility: schedule, cancel and clear scheduled notifications.
 */
public class NotificationScheduler {
    private static final String PREFS = "revise_notifications_prefs";
    private static final String KEY_IDS = "scheduled_ids";

    public static void scheduleNotification(Context ctx, String content, long timestampMs, int notificationId) {
        AlarmManager am = (AlarmManager) ctx.getSystemService(Context.ALARM_SERVICE);

        Intent intent = new Intent(ctx, NotificationPublisher.class);
        intent.putExtra(NotificationPublisher.EXTRA_NOTIFICATION_TEXT, content);
        intent.putExtra(NotificationPublisher.EXTRA_NOTIFICATION_ID, notificationId);

        PendingIntent pi = PendingIntent.getBroadcast(ctx, notificationId, intent, pendingFlags());

        addScheduledId(ctx, notificationId);

        if (am != null) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                am.setExactAndAllowWhileIdle(AlarmManager.RTC_WAKEUP, timestampMs, pi);
            } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
                am.setExact(AlarmManager.RTC_WAKEUP, timestampMs, pi);
            } else {
                am.set(AlarmManager.RTC_WAKEUP, timestampMs, pi);
            }
        }
    }

    public static void cancelNotification(Context ctx, int notificationId) {
        AlarmManager am = (AlarmManager) ctx.getSystemService(Context.ALARM_SERVICE);

        Intent intent = new Intent(ctx, NotificationPublisher.class);
        PendingIntent pi = PendingIntent.getBroadcast(ctx, notificationId, intent, pendingFlags());
        if (am != null) {
            am.cancel(pi);
        }

        NotificationManager nm = (NotificationManager) ctx.getSystemService(Context.NOTIFICATION_SERVICE);
        if (nm != null) nm.cancel(notificationId);

        removeScheduledId(ctx, notificationId);
    }

    public static void clearAllScheduledNotifications(Context ctx) {
        Set<String> ids = getScheduledIds(ctx);
        for (String s : ids) {
            try {
                int id = Integer.parseInt(s);
                cancelNotification(ctx, id);
            } catch (NumberFormatException ignored) {}
        }
        saveScheduledIds(ctx, new HashSet<String>());
    }

    // --- SharedPreferences helpers ---
    private static Set<String> getScheduledIds(Context ctx) {
        SharedPreferences prefs = ctx.getSharedPreferences(PREFS, Context.MODE_PRIVATE);
        return new HashSet<>(prefs.getStringSet(KEY_IDS, new HashSet<String>()));
    }

    private static void saveScheduledIds(Context ctx, Set<String> ids) {
        SharedPreferences prefs = ctx.getSharedPreferences(PREFS, Context.MODE_PRIVATE);
        prefs.edit().putStringSet(KEY_IDS, ids).apply();
    }

    private static void addScheduledId(Context ctx, int id) {
        Set<String> ids = getScheduledIds(ctx);
        ids.add(Integer.toString(id));
        saveScheduledIds(ctx, ids);
    }

    private static void removeScheduledId(Context ctx, int id) {
        Set<String> ids = getScheduledIds(ctx);
        ids.remove(Integer.toString(id));
        saveScheduledIds(ctx, ids);
    }

    private static int pendingFlags() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            return PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_MUTABLE;
        } else {
            return PendingIntent.FLAG_UPDATE_CURRENT;
        }
    }
}
