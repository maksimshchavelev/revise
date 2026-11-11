// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

package org.maksimshchavelev.revise;

import android.app.Activity;
import android.view.View;
import android.view.WindowInsets;
import android.view.View;
import android.view.WindowInsetsAnimation;

import java.util.List;

/**
 * Provides soft keyboard geometry using Android WindowInsets.
 *
 * The class reports IME insets during keyboard animations and forwards
 * the values to the native AndroidKeyboardInsets instance.
 */
public final class KeyboardInsets {
    private static final String TAG = "KeyboardInsets";

    private KeyboardInsets() {
    }

    /**
     * Installs the keyboard insets listener on the Activity decor view.
     *
     * @param activity Android Activity.
     * @param nativeHandle Native AndroidKeyboardInsets pointer.
     * @return True if the listener was installed successfully.
     */
    public static boolean install(Activity activity, long nativeHandle) {
        if (activity == null) {
            return false;
        }

        View root = activity.findViewById(android.R.id.content);

        if (root == null) {
            return false;
        }

        root.setOnApplyWindowInsetsListener((view, insets) -> {
            dispatch(nativeHandle, insets);

            return view.onApplyWindowInsets(insets);
        });

        root.setWindowInsetsAnimationCallback(
            new WindowInsetsAnimation.Callback(
                WindowInsetsAnimation.Callback.DISPATCH_MODE_CONTINUE_ON_SUBTREE
            ) {
                @Override
                public void onPrepare(WindowInsetsAnimation animation) {
                }

                @Override
                public WindowInsetsAnimation.Bounds onStart(
                    WindowInsetsAnimation animation,
                    WindowInsetsAnimation.Bounds bounds
                ) {
                    return bounds;
                }

                @Override
                public WindowInsets onProgress(
                    WindowInsets insets,
                    List<WindowInsetsAnimation> runningAnimations
                ) {
                    dispatch(nativeHandle, insets);
                    return insets;
                }

                @Override
                public void onEnd(WindowInsetsAnimation animation) {
                    WindowInsets insets = root.getRootWindowInsets();

                    if (insets != null) {
                        dispatch(nativeHandle, insets);
                    }
                }
            }
        );

        root.post(() -> {
            WindowInsets insets = root.getRootWindowInsets();

            if (insets != null) {
                dispatch(nativeHandle, insets);
            }

            root.requestApplyInsets();
        });

        return true;
    }

    /**
     * Forwards the current IME insets to the native implementation.
     *
     * @param nativeHandle Native AndroidKeyboardInsets pointer.
     * @param insets Current window insets.
     */
    private static void dispatch(long nativeHandle, WindowInsets insets) {
        final boolean visible = insets.isVisible(WindowInsets.Type.ime());

        final int bottomPx = insets.getInsets(
            WindowInsets.Type.ime()
        ).bottom;

        nativeOnImeInsetsChanged(nativeHandle, bottomPx, visible);
    }

    /**
     * Native callback implemented in AndroidKeyboardInsets.cpp.
     */
    private static native void nativeOnImeInsetsChanged(
        long nativeHandle,
        int bottomPx,
        boolean visible
    );
}
