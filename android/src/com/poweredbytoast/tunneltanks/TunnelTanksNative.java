package com.poweredbytoast.tunneltanks;

import android.graphics.Bitmap;

public class TunnelTanksNative {
  
  static {
    System.loadLibrary("ctunneltank");
  }
  
  private TunnelTanksNative() {}
  
  public static native void setTouchPos(int x, int y) ;
  public static native void setIsTouching(int isTouching) ;
  public static native void setDirection(int x, int y) ;
  public static native void setPress(int isPressing) ;
  public static native int  gameStep(Bitmap bmp) ;
  
}
