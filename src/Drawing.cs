using System;
using System.Drawing;

public class Drawing : IDisposable
{
    private readonly Graphics graphics;

    private readonly Bitmap bitmap;

    private readonly string path;

    public Drawing(string path, int width, int height)
    {
        this.path = path;
        this.bitmap = new Bitmap(width, height);
        this.graphics = Graphics.FromImage(bitmap);
    }

    public void Clear(Color color) => graphics.Clear(color);

    public void DrawRectangle(Pen pen, Rectangle rect) => graphics.DrawRectangle(pen, rect.X, rect.Y, rect.Width, rect.Height);

    public void Dispose()
    {
        bitmap.Save(path);
        graphics.Dispose();
        bitmap.Dispose();

        GC.SuppressFinalize(this);
    }
}