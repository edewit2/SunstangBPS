package com.example.gpslogger;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;

import android.location.Criteria;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;
import gs.par.gpslogger.R;

public class MainActivity extends Activity implements LocationListener, OnClickListener {

	private TextView latituteField;
	private TextView longitudeField;
	private EditText fileNameField;
	private ToggleButton start;
	private LocationManager locationManager;
	private String provider;
	
	private boolean toFile;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
	    setContentView(R.layout.activity_main);
	    latituteField = (TextView) findViewById(R.id.textView1);
	    longitudeField = (TextView) findViewById(R.id.textView2);
	    start = (ToggleButton) findViewById(R.id.toggleButton1);
	    fileNameField = (EditText) findViewById(R.id.fileNameField);
	    
	    start.setOnClickListener(this);
	    locationManager = (LocationManager) getSystemService(Context.LOCATION_SERVICE);

	    Criteria criteria = new Criteria();
	    provider = locationManager.getBestProvider(criteria, false);
	    Location location = locationManager.getLastKnownLocation(provider);

	    // Initialize the location fields
	    if (location != null) {
	      System.out.println("Provider " + provider + " has been selected.");
	      onLocationChanged(location);
	    } else {
	      latituteField.setText("Location not available");
	      longitudeField.setText("Location not available");
	    }
}

	  /* Request updates at startup */
	  @Override
	  protected void onResume() {
	    super.onResume();
	    locationManager.requestLocationUpdates(provider, 400, 1, this);
	  }

	  /* Remove the locationlistener updates when Activity is paused */
	  @Override
	  protected void onPause() {
	    super.onPause();
	    locationManager.removeUpdates(this);
	  }

	  @Override
	  public void onLocationChanged(Location location) {
	    double lat =  (location.getLatitude());
	    double lng =  (location.getLongitude());
	    latituteField.setText(String.valueOf(lat));
	    longitudeField.setText(String.valueOf(lng));
	    if(toFile){
	    	File sdcard = Environment.getExternalStorageDirectory();
	    	File file = new File(sdcard, fileNameField.getText().toString());
			
	    	String val = location.getTime() +"," + lat +',' +lng + ',' + location.getAccuracy();
	    	
	        try {
	           BufferedWriter out = new BufferedWriter(new FileWriter(file, true));
	           out.write(val);
	           out.newLine();
	           out.flush();
	           out.close();
	           Toast.makeText(this, "Saved to File", Toast.LENGTH_SHORT).show();
	        	
	     } catch (IOException e) {
	    	 Toast.makeText(this,"IO error", Toast.LENGTH_SHORT).show();
		}
	    }
	    
	  }

	  @Override
	  public void onStatusChanged(String provider, int status, Bundle extras) {
	    // TODO Auto-generated method stub

	  }

	  @Override
	  public void onProviderEnabled(String provider) {
	    Toast.makeText(this, "Enabled new provider " + provider,
	        Toast.LENGTH_SHORT).show();

	  }

	  @Override
	  public void onProviderDisabled(String provider) {
	    Toast.makeText(this, "Disabled provider " + provider,
	        Toast.LENGTH_SHORT).show();
	  }

	@Override
	public void onClick(View v) {
		toFile = ((ToggleButton)v).isChecked();
		fileNameField.setEnabled(!toFile);
	}

}
