using System;
using System.Runtime.CompilerServices;

public struct Vector2
{
    public float x { get; }
    public float y { get; }

    public static Vector2 zero { get { return new Vector2(0, 0); } }
    public static Vector2 right { get { return new Vector2(1.0f, 0); } }
    public static Vector2 up { get { return new Vector2(0, 1.0f); } }

    public Vector2(float x, float y)
    {
        this.x = x;
        this.y = y;
    }

    public static Vector2 operator +(Vector2 a, Vector2 b)
    {
        return new Vector2(a.x + b.x, a.y + b.y);
    }

    public static Vector2 operator -(Vector2 a, Vector2 b)
    {
        return new Vector2(a.x - b.x, a.y - b.y);
    }

    public static Vector2 operator *(Vector2 a, float b)
    {
        return new Vector2(a.x * b, a.y * b);
    }

    public static Vector2 operator /(Vector2 a, float b)
    {
        return new Vector2(a.x / b, a.y / b);
    }

    float Magnitude()
    {
        return (float)Math.Sqrt(x * x + y * y);
    }

    public Vector2 Normalize()
    {
        float mag = Magnitude();

        if (mag == 0) { return Vector2.zero; }

        return new Vector2(x, y) / Magnitude();
    }

    public static float Distance(Vector2 a, Vector2 b)
    {
        Vector2 resultVector = a - b;

        return resultVector.Magnitude();
    }
}