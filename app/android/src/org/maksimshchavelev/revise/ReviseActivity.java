// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

package org.maksimshchavelev.revise;

import android.os.Bundle;
import android.view.Display;
import android.view.Window;
import android.view.WindowManager;

import org.qtproject.qt.android.bindings.QtActivity;

public final class ReviseActivity extends QtActivity {

    private static final String TAG = "ReviseActivity";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        setTheme(R.style.ReviseTheme);

        super.onCreate(savedInstanceState);

        requestMaximumRefreshRate();
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
}
