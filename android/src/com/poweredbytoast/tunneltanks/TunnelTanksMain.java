package com.poweredbytoast.tunneltanks;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.os.PowerManager;

public class TunnelTanksMain extends Activity {
  
  // Basically just a fancy bitmap:
  private TunnelTanksCanvas ttc;
  
  // Keeps the screen on:
  private PowerManager.WakeLock lock;
  
  /** Called when the activity is first created. */
  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    
    PowerManager pm = (PowerManager)getSystemService(Context.POWER_SERVICE);
    lock = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK | PowerManager.ON_AFTER_RELEASE, "TunnelTanksMain");
    
    ttc = new TunnelTanksCanvas(this);
    ttc.setFocusable(true);
    ttc.setFocusableInTouchMode(true);
    setContentView(ttc);
  }
  
  @Override
  public void onResume() {
    super.onResume();
    lock.acquire();
  }
  
  @Override
  public void onPause() {
    super.onPause();
    lock.release();
  }
}
