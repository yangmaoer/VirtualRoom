﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace EaseIn
{
   public partial class Form1 : Form
   {
      public Form1()
      {
         InitializeComponent();
         this.SetStyle(ControlStyles.DoubleBuffer, true);
         this.SetStyle(ControlStyles.AllPaintingInWmPaint, true);
      }

      public enum SLEasingCurve
      {
         linear,
         inQuad,  outQuad,  inOutQuad,  outInQuad,
         inCubic, outCubic, inOutCubic, outInCubic,
         inQuart, outQuart, inOutQuart, outInQuart,
         inQuint, outQuint, inOutQuint, outInQuint,
         inSine,  outSine,  inOutSine,  outInSine
      }

      private void Form1_Paint(object sender, PaintEventArgs e)
      {
         DrawEasingCurve(e.Graphics, SLEasingCurve.linear, Pens.Black);

         DrawEasingCurve(e.Graphics, SLEasingCurve.inQuad,      Pens.Red);
         DrawEasingCurve(e.Graphics, SLEasingCurve.outQuad,     Pens.Blue);
         DrawEasingCurve(e.Graphics, SLEasingCurve.inOutQuad,   Pens.Magenta);
         DrawEasingCurve(e.Graphics, SLEasingCurve.outInQuad,   Pens.Cyan);

         DrawEasingCurve(e.Graphics, SLEasingCurve.inCubic,     Pens.Red);
         DrawEasingCurve(e.Graphics, SLEasingCurve.outCubic,    Pens.Blue);
         DrawEasingCurve(e.Graphics, SLEasingCurve.inOutCubic,  Pens.Magenta);
         DrawEasingCurve(e.Graphics, SLEasingCurve.outInCubic,  Pens.Cyan);

         DrawEasingCurve(e.Graphics, SLEasingCurve.inQuart,     Pens.Red);
         DrawEasingCurve(e.Graphics, SLEasingCurve.outQuart,    Pens.Blue);
         DrawEasingCurve(e.Graphics, SLEasingCurve.inOutQuart,  Pens.Magenta);
         DrawEasingCurve(e.Graphics, SLEasingCurve.outInQuart,  Pens.Cyan);

         DrawEasingCurve(e.Graphics, SLEasingCurve.inQuint,     Pens.Red);
         DrawEasingCurve(e.Graphics, SLEasingCurve.outQuint,    Pens.Blue);
         DrawEasingCurve(e.Graphics, SLEasingCurve.inOutQuint,  Pens.Magenta);
         DrawEasingCurve(e.Graphics, SLEasingCurve.outInQuint,  Pens.Cyan);

         DrawEasingCurve(e.Graphics, SLEasingCurve.inSine,      Pens.Red);
         DrawEasingCurve(e.Graphics, SLEasingCurve.outSine,     Pens.Blue);
         DrawEasingCurve(e.Graphics, SLEasingCurve.inOutSine,   Pens.Magenta);
         DrawEasingCurve(e.Graphics, SLEasingCurve.outInSine,   Pens.Cyan);

      }

      private void Form1_Resize(object sender, EventArgs e)
      {
         this.Width = this.Height;
         this.Invalidate();
      }

      private void DrawEasingCurve(Graphics g, SLEasingCurve type, Pen penCol)
      {  
         int steps = 100;
         PointF[] p = new PointF[steps];
         float w = this.ClientRectangle.Width;
         float h = this.ClientRectangle.Height;

         float dt = 1 / (float)p.Length;
         for (int i=0; i<p.Length; ++i)
         {  float t = i*dt;
            p[i].X = t*w;
            p[i].Y = h - Easing(type, t) * h;
         }
         
         g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;
         g.DrawLines(penCol, p);
      }

      private float Easing(SLEasingCurve type, float x) 
      {
         switch (type)
         {
            case SLEasingCurve.linear:      return   x;

            case SLEasingCurve.inQuad:      return                 (float)Math.Pow(x     ,2.0f);
            case SLEasingCurve.outQuad:     return                -(float)Math.Pow(x-1.0f,2.0f) + 1.0f;
            case SLEasingCurve.inOutQuad:   return (x<0.5f) ? 2.0f*(float)Math.Pow(x     ,2.0f) : 
                                                             -2.0f*(float)Math.Pow(x-1.0f,2.0f) + 1.0f;
            case SLEasingCurve.outInQuad:   return (x<0.5f) ?-2.0f*(float)Math.Pow(x-0.5f,2.0f) + 0.5f : 
                                                              2.0f*(float)Math.Pow(x-0.5f,2.0f) + 0.5f;
   
            case SLEasingCurve.inCubic:     return                 (float)Math.Pow(x     ,3.0f);
            case SLEasingCurve.outCubic:    return                 (float)Math.Pow(x-1.0f,3.0f) + 1.0f;
            case SLEasingCurve.inOutCubic:  return (x<0.5f) ? 4.0f*(float)Math.Pow(x     ,3.0f) : 
                                                              4.0f*(float)Math.Pow(x-1.0f,3.0f) + 1.0f;
            case SLEasingCurve.outInCubic:  return            4.0f*(float)Math.Pow(x-0.5f,3.0f) + 0.5f;

            case SLEasingCurve.inQuart:     return                 (float)Math.Pow(x     ,4.0f);
            case SLEasingCurve.outQuart:    return                -(float)Math.Pow(x-1.0f,4.0f) + 1.0f;
            case SLEasingCurve.inOutQuart:  return (x<0.5f) ? 8.0f*(float)Math.Pow(x     ,4.0f) : 
                                                             -8.0f*(float)Math.Pow(x-1.0f,4.0f) + 1.0f;
            case SLEasingCurve.outInQuart:  return (x<0.5f) ?-8.0f*(float)Math.Pow(x-0.5f,4.0f) + 0.5f : 
                                                              8.0f*(float)Math.Pow(x-0.5f,4.0f) + 0.5f;
   
            case SLEasingCurve.inQuint:     return                 (float)Math.Pow(x     ,5.0f);
            case SLEasingCurve.outQuint:    return                 (float)Math.Pow(x-1.0f,5.0f) + 1.0f;
            case SLEasingCurve.inOutQuint:  return (x<0.5f) ?16.0f*(float)Math.Pow(x     ,5.0f) : 
                                                             16.0f*(float)Math.Pow(x-1.0f,5.0f) + 1.0f;
            case SLEasingCurve.outInQuint:  return           16.0f*(float)Math.Pow(x-0.5f,5.0f) + 0.5f;

            case SLEasingCurve.inSine:      return                 (float)Math.Sin(x*Math.PI*0.5 - Math.PI*0.5) + 1.0f;
            case SLEasingCurve.outSine:     return                 (float)Math.Sin(x*Math.PI*0.5);
            case SLEasingCurve.inOutSine:   return            0.5f*(float)Math.Sin(x*Math.PI - Math.PI*0.5) + 0.5f;
            case SLEasingCurve.outInSine:   return (x<0.5f) ? 0.5f*(float)Math.Sin(x*Math.PI) : 
                                                              0.5f*(float)Math.Sin(x*Math.PI - Math.PI) + 1.0f;                                  
            default: return x;
         }
      }
   }
}
