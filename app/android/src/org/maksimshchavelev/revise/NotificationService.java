package org.maksimshchavelev.revise;

import android.app.Activity;
import android.app.AlarmManager;
import android.app.Notification;
import android.app.PendingIntent;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Build;

import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationManagerCompat;

import java.util.HashSet;
import java.util.Set;

/**
 * JNI-friendly service used from C++ to manage notifications.
 *
 * Usage from C++: create instance passing Activity (or Context that is Activity), then call methods.
 */
public class NotificationService {
    private static final String PREFS = "org.maksimshchavelev.revise.notifications";
    private static final String KEY_SCHEDULED_IDS = "scheduled_ids";
    private static final String CHANNEL_ID = "revise_default_channel";
    private static final String ACTION_ALARM = "org.maksimshchavelev.revise.ACTION_ALARM";

    private final Context context;
    private final AlarmManager alarmManager;
    private final SharedPreferences prefs;

    /**
     * Construct with an Android Context (preferably Activity).
     * @param ctx Activity or app Context — but Activity is recommended for some PendingIntent behaviors.
     */
    public NotificationService(Context ctx) {
        this.context = ctx.getApplicationContext();
        this.alarmManager = (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);
        this.prefs = context.getSharedPreferences(PREFS, Context.MODE_PRIVATE);
        createNotificationChannelIfNeeded();
    }

    /**
     * Show a notification right now.
     * @param text Notification text.
     */
    public void showNotification(String text) {
        int id = generateIdForImmediate();
        showNotificationInternal(id, text);
    }

    /**
     * Public wrapper to show a notification with a specific id.
     * This is used by BroadcastReceivers (AlarmReceiver) or any external caller.
     * @param id notification id to use (also used as PendingIntent requestCode)
     * @param text notification text
     */
    public void showNotificationWithId(int id, String text) {
        showNotificationInternal(id, text);
    }

    /**
     * Schedule notification at a given epoch millis (UTC millis).
     * @param text Notification text.
     * @param epochMillis Time in millis since epoch (System.currentTimeMillis() style).
     */
    public void scheduleNotification(String text, long epochMillis) {
        int requestCode = genRequestCode();
        // store id so we can cancel later
        addScheduledId(requestCode);

        Intent alarmIntent = new Intent(context, AlarmReceiver.class);
        alarmIntent.setAction(ACTION_ALARM);
        alarmIntent.putExtra("notification_text", text);
        alarmIntent.putExtra("notification_id", requestCode);

        int flags = PendingIntent.FLAG_UPDATE_CURRENT;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            flags |= PendingIntent.FLAG_IMMUTABLE;
        }

        PendingIntent pi = PendingIntent.getBroadcast(
            context,
            requestCode,
            alarmIntent,
            flags
        );

        // Use exact alarm when possible
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            alarmManager.setExactAndAllowWhileIdle(AlarmManager.RTC_WAKEUP, epochMillis, pi);
        } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            alarmManager.setExact(AlarmManager.RTC_WAKEUP, epochMillis, pi);
        } else {
            alarmManager.set(AlarmManager.RTC_WAKEUP, epochMillis, pi);
        }
    }

    /**
     * Cancel all scheduled notifications created by app and remove their persisted ids.
     */
    public void clearAllScheduledNotifications() {
        Set<String> ids = getScheduledIdStrings();
        for (String s : ids) {
            try {
                int requestCode = Integer.parseInt(s);
                Intent alarmIntent = new Intent(context, AlarmReceiver.class);
                alarmIntent.setAction(ACTION_ALARM);
                PendingIntent pi = PendingIntent.getBroadcast(
                        context,
                        requestCode,
                        alarmIntent,
                        pendingIntentFlags() // must match
                );
                alarmManager.cancel(pi);
                // also cancel any shown notification with same id
                NotificationManagerCompat.from(context).cancel(requestCode);
            } catch (NumberFormatException ignored) { }
        }
        // clear prefs
        prefs.edit().remove(KEY_SCHEDULED_IDS).apply();
    }

    /* ================= internal helpers ================= */

    private void showNotificationInternal(int id, String text) {
        // Build an intent that opens the app when notification is tapped.
        Intent launchIntent = context.getPackageManager().getLaunchIntentForPackage(context.getPackageName());
        if (launchIntent == null) {
            // Fallback — open main activity by class name (if you have one, replace with concrete class)
            launchIntent = new Intent(Intent.ACTION_MAIN);
            launchIntent.addCategory(Intent.CATEGORY_LAUNCHER);
            launchIntent.setPackage(context.getPackageName());
        }
        launchIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TOP);

        PendingIntent contentIntent = PendingIntent.getActivity(
                context,
                id, // request code
                launchIntent,
                pendingIntentFlags() | PendingIntent.FLAG_UPDATE_CURRENT
        );

        NotificationCompat.Builder nb = new NotificationCompat.Builder(context, CHANNEL_ID)
                .setSmallIcon(android.R.drawable.ic_dialog_info)
                .setContentTitle(context.getApplicationInfo().loadLabel(context.getPackageManager()).toString())
                .setContentText(text)
                .setAutoCancel(true)
                .setContentIntent(contentIntent)
                .setPriority(NotificationCompat.PRIORITY_HIGH);

        Notification notification = nb.build();
        NotificationManagerCompat.from(context).notify(id, notification);
    }

    private int generateIdForImmediate() {
        // Use timestamp low bits as id
        return (int) (System.currentTimeMillis() & 0x7FFFFFFF);
    }

    private int genRequestCode() {
        // request code for PendingIntent — keep within int range
        return (int) (System.currentTimeMillis() & 0x7FFFFFFF);
    }

    private int pendingIntentFlags() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            return PendingIntent.FLAG_IMMUTABLE;
        } else {
            return PendingIntent.FLAG_UPDATE_CURRENT;
        }
    }

    private void addScheduledId(int id) {
        Set<String> ids = getScheduledIdStrings();
        ids.add(Integer.toString(id));
        prefs.edit().putStringSet(KEY_SCHEDULED_IDS, ids).apply();
    }

    private Set<String> getScheduledIdStrings() {
        Set<String> s = prefs.getStringSet(KEY_SCHEDULED_IDS, null);
        if (s == null) return new HashSet<>();
        // return a copy (because some Android versions return an unmodifiable set)
        return new HashSet<>(s);
    }

    private void createNotificationChannelIfNeeded() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            CharSequence name = "Revise notifications";
            String description = "Notifications for Revise app";
            int importance = android.app.NotificationManager.IMPORTANCE_DEFAULT;
            android.app.NotificationChannel channel = new android.app.NotificationChannel(CHANNEL_ID, name, importance);
            channel.setDescription(description);
            android.app.NotificationManager nm = context.getSystemService(android.app.NotificationManager.class);
            if (nm != null) nm.createNotificationChannel(channel);
        }
    }
}
