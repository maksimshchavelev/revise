package org.maksimshchavelev.revise;

import android.content.pm.ActivityInfo;
import android.os.Bundle;

import org.qtproject.qt.android.bindings.QtActivity;

public final class ReviseActivity extends QtActivity {

    private static final int TABLET_SMALLEST_WIDTH_DP = 600;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        setTheme(R.style.ReviseTheme);

        final int smallestScreenWidthDp =
            getResources().getConfiguration().smallestScreenWidthDp;

        if (smallestScreenWidthDp >= TABLET_SMALLEST_WIDTH_DP) {
            setRequestedOrientation(
                ActivityInfo.SCREEN_ORIENTATION_UNSPECIFIED
            );
        } else {
            setRequestedOrientation(
                ActivityInfo.SCREEN_ORIENTATION_PORTRAIT
            );
        }

        super.onCreate(savedInstanceState);
    }
}
