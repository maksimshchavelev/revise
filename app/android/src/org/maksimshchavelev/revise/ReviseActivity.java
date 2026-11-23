
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

    private static final int STATUS_BAR_COLOR = Color.rgb(10, 16, 30);

    @Override
    public void onCreate(Bundle savedInstanceState) {
        setTheme(R.style.ReviseTheme);

        super.onCreate(savedInstanceState);

        requestMaximumRefreshRate();
        configureStatusBar();
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
     * Configures the status bar appearance for the dark application theme.
     *
     * The window background provides a fallback color when the system
     * enforces edge-to-edge rendering and makes the status bar transparent.
     */
    private void configureStatusBar() {
        final Window window = getWindow();

        window.setStatusBarColor(STATUS_BAR_COLOR);
        window.setBackgroundDrawable(
            new ColorDrawable(STATUS_BAR_COLOR)
        );

        final WindowInsetsController controller =
            window.getInsetsController();

        if (controller != null) {
            controller.setSystemBarsAppearance(
                0,
                WindowInsetsController.APPEARANCE_LIGHT_STATUS_BARS
            );
        }
    }
}
