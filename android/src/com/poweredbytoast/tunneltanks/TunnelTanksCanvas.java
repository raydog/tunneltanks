package com.poweredbytoast.tunneltanks;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.os.SystemClock;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;

public class TunnelTanksCanvas extends View {
  Bitmap bmp;
  
  public TunnelTanksCanvas(Context context) {
    super(context);
    
    // Create a default bmp, just for giggles:
    bmp = Bitmap.createBitmap(600, 400, Bitmap.Config.RGB_565);
  }
  
  long old_time = 0, frames = 0;
  
  private void handleFPS() {
    long new_time = System.currentTimeMillis() / 1000L;
    frames++;
    if(new_time != old_time) {
      Log.i("GameCanvas.handleFPS", "Current FPS: " + frames);
      old_time = new_time; frames = 0;
    }
  }
  
  static final int GAME_FPS = 24;
  static final int GAME_FPS_WAIT = 1000/GAME_FPS;
  private long lastTime = 0;
  
  private void smartWait() {
    long cur = System.currentTimeMillis();
    long next = (cur/GAME_FPS_WAIT + 1) * GAME_FPS_WAIT;
    
    if(cur - lastTime > GAME_FPS_WAIT) {
      lastTime = next;
      return;
    }
    
    lastTime = next;
    SystemClock.sleep(next - cur);
  }
  
  @Override
  public void onSizeChanged(int w, int h, int oldW, int oldH) {
    Bitmap newBmp = Bitmap.createBitmap(w, h, Bitmap.Config.RGB_565);
    bmp.recycle();
    bmp = newBmp;
  }
  
  @Override
  public void onDraw(Canvas c) {
    TunnelTanksNative.gameStep(bmp);
    c.drawBitmap(bmp, 0, 0, null);
    handleFPS();
    smartWait();
    invalidate();
  }
  
  @Override
  public boolean onTouchEvent(MotionEvent e) {
    if(e.getAction()==MotionEvent.ACTION_CANCEL ||
       e.getAction()==MotionEvent.ACTION_UP     ||
       e.getAction()==MotionEvent.ACTION_OUTSIDE)
      TunnelTanksNative.setIsTouching(0);
    else
      TunnelTanksNative.setIsTouching(1);
    TunnelTanksNative.setTouchPos((int)e.getX(), (int)e.getY());
    return true;
  }
  
  @Override
  public boolean onTrackballEvent(MotionEvent e) {
    if(e.getAction() == MotionEvent.ACTION_DOWN)
      TunnelTanksNative.setPress(1);
    else if(e.getAction() == MotionEvent.ACTION_UP)
      TunnelTanksNative.setPress(0);
    return true;
  }

}
