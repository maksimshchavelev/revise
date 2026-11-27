// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

package org.maksimshchavelev.revise;

import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.view.Display;
import android.view.Window;
import android.view.WindowInsetsController;
import android.view.WindowManager;

import org.qtproject.qt.android.bindings.QtActivity;

public final class ReviseActivity extends QtActivity {

    private static final String TAG = "ReviseActivity";

    private static final int SYSTEM_BARS_APPEARANCE_MASK =
        WindowInsetsController.APPEARANCE_LIGHT_STATUS_BARS
        | WindowInsetsController.APPEARANCE_LIGHT_NAVIGATION_BARS;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        setTheme(R.style.ReviseTheme);

        super.onCreate(savedInstanceState);

        requestMaximumRefreshRate();
        configureSystemBars();
    }

    @Override
    protected void onPostResume() {
        super.onPostResume();

        configureSystemBars();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);

        if (hasFocus) {
            configureSystemBars();
        }
    }

    /**
     * Requests the highest refresh rate supported by the current display.
     *
     * The system may ignore this preference depending on display settings,
     * power management policies, or device-specific restrictions.
     */
    private void requestMaximumRefreshRate() {
        final Display display = getDisplay();

        if (display == null) {
            return;
        }

        float maximumRefreshRate = 0.0f;

        for (Display.Mode mode : display.getSupportedModes()) {
            maximumRefreshRate = Math.max(
                maximumRefreshRate,
                mode.getRefreshRate()
            );
        }

        if (maximumRefreshRate <= 0.0f) {
            return;
        }

        final Window window = getWindow();

        final WindowManager.LayoutParams attributes =
            window.getAttributes();

        attributes.preferredRefreshRate = maximumRefreshRate;

        window.setAttributes(attributes);
    }

    /**
     * Configures black system bars with light-colored system indicators.
     *
     * On Android 15 and later, system bar colors are provided by the
     * application content drawn behind the transparent system bars.
     * The window background acts as a fallback outside the Qt surface.
     */
    @SuppressWarnings("deprecation")
    private void configureSystemBars() {
        final Window window = getWindow();

        window.addFlags(
            WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS
        );

        window.clearFlags(
            WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS
            | WindowManager.LayoutParams.FLAG_TRANSLUCENT_NAVIGATION
        );

        // Legacy fallback for Android versions without enforced edge-to-edge.
        window.setStatusBarColor(Color.BLACK);
        window.setNavigationBarColor(Color.BLACK);

        window.setStatusBarContrastEnforced(false);
        window.setNavigationBarContrastEnforced(false);

        window.setBackgroundDrawable(
            new ColorDrawable(Color.BLACK)
        );

        window.getDecorView().post(() -> {
            final WindowInsetsController controller =
                window.getInsetsController();

            if (controller == null) {
                return;
            }

            // Clear both light-system-bar flags to request light indicators.
            controller.setSystemBarsAppearance(
                0,
                SYSTEM_BARS_APPEARANCE_MASK
            );
        });
    }
}
