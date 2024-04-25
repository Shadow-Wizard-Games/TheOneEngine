using System;

public struct Color
{
    public float r { get; }
    public float g { get; }
    public float b { get; }
    public float a { get; }

    public static Color mondongoBlack   { get { return new Color(0, 0, 0); } }
    public static Color foreignerWhite  { get { return new Color(1, 1, 1); } }
    public static Color russianRed      { get { return new Color(1, 0, 0); } }
    public static Color chernobylGreen  { get { return new Color(0, 1, 0); } }
    public static Color asianYellow     { get { return new Color(1, 1, 0.2f); } }
    public static Color scrumMasterBlue { get { return new Color(0, 0, 1); } }
    public static Color pitufoBlue      { get { return new Color(0.2f, 0.6f, 1); } }
    public static Color bimbibambaPurple { get { return new Color(0.6f, 0.2f, 1); } }
    public static Color peruvianBrown   { get { return new Color(0.36f, 0.21f, 0); } }
    public static Color coquettePink    { get { return new Color(1, 0.5f, 0.85f); } }
    public static Color trumpOrange     { get { return new Color(1, 0.6f, 0); } }

    /// Max value of 1.0.
    public Color(float r, float g, float b, float a = 1.0f)
    {
        this.r = r;
        this.g = g;
        this.b = b;
        this.a = a;
    }
    /// Max value of 255.
    public Color(int r, int g, int b, int a = 255)
    {
        this.r = r / 255.0f;
        this.g = g / 255.0f;
        this.b = b / 255.0f;
        this.a = a / 255.0f;
    }

    public static bool operator ==(Color a, Color b)
    {
        return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
    }

    public static bool operator !=(Color a, Color b)
    {
        return a.r != b.r || a.g != b.g || a.b != b.b || a.a != b.a;
    }

    public Color To255()
    {
        return new Color((int)Math.Round(r * 255.0f),
                         (int)Math.Round(g * 255.0f),
                         (int)Math.Round(b * 255.0f),
                         (int)Math.Round(a * 255.0f));
    }

    public Vector3 ToVector3()
    {
        return new Vector3(r, g, b);
    }
}
