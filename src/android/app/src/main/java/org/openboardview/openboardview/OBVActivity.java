package org.openboardview.openboardview;

import org.libsdl.app.SDLActivity;
import android.Manifest;
import android.os.Build;
import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.content.UriPermission;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import android.net.Uri;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.ByteArrayOutputStream;

public class OBVActivity extends SDLActivity {
	private static final int FILE_SELECT_CODE = 0;
	private static final String TAG = "[OBV]";
	private static OBVActivity activity;

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

		Intent fileIntent;
		if (Build.VERSION.SDK_INT < Build.VERSION_CODES.KITKAT) {
			fileIntent = new Intent(Intent.ACTION_GET_CONTENT);
		} else {
			fileIntent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
		}
		fileIntent.setType("*/*"); // intent type to filter application based on your requirement
		fileIntent.addCategory(Intent.CATEGORY_OPENABLE);
		try {
			activity.startActivityForResult(Intent.createChooser(fileIntent, "Select a File to Upload"), FILE_SELECT_CODE);
		} catch (android.content.ActivityNotFoundException ex) {
			Toast.makeText(activity, "Please install a File Manager.", Toast.LENGTH_SHORT).show();
		}
	}

	public static byte[] readFile(String suri) {
		try {
			Uri uri = Uri.parse(suri);

			if (!activity.takePersistentPerms(uri)) {
				Log.e(TAG, "Persistent read permission for " + uri + " not granted.");
				return new byte[0];
			}

			InputStream inputStream = activity.getContentResolver().openInputStream(uri);

			ByteArrayOutputStream buffer = new ByteArrayOutputStream();
			int nRead;
			byte[] data = new byte[1024];

			while ((nRead = inputStream.read(data, 0, data.length)) != -1) {
				buffer.write(data, 0, nRead);
			}

			buffer.flush();
			return buffer.toByteArray();
		} catch (IOException e) {
			Log.e(TAG, "Error while reading " + suri + ": " + e);
			return new byte[0];
		}
	}

	public native void openFileWrapper(String filePath);

	@Override
	protected void onCreate(Bundle bundle) {
		this.activity = this;
		Log.d(TAG, "started");
		super.onCreate(bundle);
		checkPerms();
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (requestCode == FILE_SELECT_CODE) {
			if (resultCode == RESULT_OK) {
				// Get the Uri of the selected file
				Uri uri = data.getData();
				openFileWrapper(uri.toString());
			}
		}
		super.onActivityResult(requestCode, resultCode, data);
	}

	boolean takePersistentPerms(Uri uri) {
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
			getContentResolver().takePersistableUriPermission(uri, Intent.FLAG_GRANT_READ_URI_PERMISSION);

			for (UriPermission up : getContentResolver().getPersistedUriPermissions()) {
				if (up.getUri().equals(uri) && up.isReadPermission()) {
					return true;
				}
			}
			return false;
		}
		return true;
	}

	void checkPerms() {
		// No need to ask for perms before marshmallow
		if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M) {
			return;
		}

		if (checkSelfPermission(android.Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
			requestPermissions(new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 0);
		}
	}
}
