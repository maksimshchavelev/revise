package org.maksimshchavelev.revise;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Build;

import androidx.core.app.NotificationCompat;

/**
 * Receiver which actually shows the notification when AlarmManager fires.
 */
public class NotificationPublisher extends BroadcastReceiver {
    public static final String EXTRA_NOTIFICATION_ID = "notification_id";
    public static final String EXTRA_NOTIFICATION_TEXT = "notification_text";
    public static final String CHANNEL_ID = "revise_channel";
    public static final CharSequence CHANNEL_NAME = "Revise notifications";

    @Override
    public void onReceive(Context context, Intent intent) {
        String text = intent.getStringExtra(EXTRA_NOTIFICATION_TEXT);
        int id = intent.getIntExtra(EXTRA_NOTIFICATION_ID, 0);

        NotificationManager nm = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel channel = new NotificationChannel(CHANNEL_ID, CHANNEL_NAME, NotificationManager.IMPORTANCE_DEFAULT);
            channel.setDescription("Reminders from Revise");
            nm.createNotificationChannel(channel);
        }

        String title = context.getApplicationInfo().loadLabel(context.getPackageManager()).toString();

        NotificationCompat.Builder builder = new NotificationCompat.Builder(context, CHANNEL_ID)
                .setContentTitle(title)
                .setContentText(text != null ? text : "")
                .setSmallIcon(context.getApplicationInfo().icon)
                .setAutoCancel(true)
                .setPriority(NotificationCompat.PRIORITY_DEFAULT);

        Notification notification = builder.build();
        nm.notify(id, notification);
    }
}
