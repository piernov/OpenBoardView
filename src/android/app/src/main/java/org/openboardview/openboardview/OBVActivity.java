package org.openboardview.openboardview;

import org.libsdl.app.SDLActivity;
import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.util.Log;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import android.net.Uri;
import java.io.File;

public class OBVActivity extends SDLActivity {
	private static final int FILE_SELECT_CODE = 0;
	private static final String TAG = "[OBV]";
	private static OBVActivity activity;

	public native void openFileWrapper(String filePath);
	public native void onScale(float x, float y, float factor);

	private ScaleGestureDetector mScaleDetector;

	private class ScaleListener extends ScaleGestureDetector.SimpleOnScaleGestureListener {
		@Override
		public boolean onScale(ScaleGestureDetector detector) {
			//mScaleFactor *= detector.getScaleFactor();
			//SDLActivity.onNativeZoom((float)(mScaleFactor - mScaleFactorLast));
			//mScaleFactorLast = mScaleFactor;
			activity.onScale(detector.getFocusX(), detector.getFocusY(), detector.getScaleFactor());
			return true;
		}
	}

	private class OBVSurface extends SDLSurface {
	@Override
	public boolean onTouch(View v, MotionEvent event) {
		if (event.getPointerCount() > 1) { // The only multi-touch event we process is for pinch-to-zoom
			return mScaleDetector.onTouchEvent(event);
		} else {
			return super.onTouch(v, event);
		}
	}
	}

	public OBVActivity() {
		super();
		mScaleDetector = new ScaleGestureDetector(activity.getContext(), new ScaleListener());
	}

	@Override
	protected String[] getLibraries() {
		return new String[] {
			"SDL2",
			// "SDL2_image",
			// "SDL2_mixer",
			// "SDL2_net",
			// "SDL2_ttf",
			"openboardview"
		};
	}

	public static void openFilePicker() {
		Log.d(TAG, "Opening file picker");
		Intent fileIntent = new Intent(Intent.ACTION_GET_CONTENT);
		fileIntent.setType("*/*"); // intent type to filter application based on your requirement
		fileIntent.addCategory(Intent.CATEGORY_OPENABLE);
		try {
			activity.startActivityForResult(Intent.createChooser(fileIntent, "Select a File to Upload"), FILE_SELECT_CODE);
		} catch (android.content.ActivityNotFoundException ex) {
			Toast.makeText(activity, "Please install a File Manager.", Toast.LENGTH_SHORT).show();
		}
	}


	@Override
	protected void onCreate(Bundle bundle) {
		this.activity = this;
		Log.d(TAG, "started");
		super.onCreate(bundle);

		mSurface = new OBVSurface(getApplication());
		mLayout = new AbsoluteLayout(this);
		mLayout.addView(mSurface);

		setContentView(mLayout);
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (requestCode == FILE_SELECT_CODE) {
			if (resultCode == RESULT_OK) {
				// Get the Uri of the selected file
				Uri uri = data.getData();
				File file = new File(uri.getPath());
				String filePath = file.getAbsolutePath();
				openFileWrapper(filePath);
			}
		}
		super.onActivityResult(requestCode, resultCode, data);
	}

}
