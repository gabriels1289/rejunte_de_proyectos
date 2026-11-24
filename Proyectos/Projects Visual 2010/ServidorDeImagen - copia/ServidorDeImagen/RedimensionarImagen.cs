using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;

namespace ServidorDeImagen
{
    class RedimensionarImagen
    {
        //public static MemoryStream Redimensiona(Stream imagen, int targetW, int targetH, int resolucion)
        public static MemoryStream Redimensiona(Stream imagen, int targetW, int targetH)
        {
            System.Drawing.Image original = System.Drawing.Image.FromStream(imagen);
            System.Drawing.Image imgPhoto = System.Drawing.Image.FromStream(imagen);

            // No vamos a permitir que la imagen final sea más grande que la inicial
            //targetH = targetH <= original.Height ? targetH : original.Height;
            //targetW = targetW <= original.Width ? targetW : original.Width;

            Bitmap bmPhoto = new Bitmap(targetW, targetH, PixelFormat.Format24bppRgb);

            // No vamos a permitir dar una resolución mayor de la que tiene
            //resolucion = resolucion <= Convert.ToInt32(bmPhoto.HorizontalResolution) ? resolucion : Convert.ToInt32(bmPhoto.HorizontalResolution);
            //resolucion = resolucion <= Convert.ToInt32(bmPhoto.VerticalResolution) ? resolucion : Convert.ToInt32(bmPhoto.VerticalResolution);

            //bmPhoto.SetResolution(resolucion, resolucion);
            Graphics grPhoto = Graphics.FromImage(bmPhoto);

            grPhoto.SmoothingMode = SmoothingMode.AntiAlias;
            grPhoto.InterpolationMode = InterpolationMode.HighQualityBicubic;
            grPhoto.PixelOffsetMode = PixelOffsetMode.HighQuality;
            grPhoto.DrawImage(imgPhoto, new Rectangle(0, 0, targetW, targetH), new Rectangle(0, 0, original.Width, original.Height), GraphicsUnit.Pixel);

            MemoryStream mm = new MemoryStream();
            bmPhoto.Save(mm, System.Drawing.Imaging.ImageFormat.Bmp);

            original.Dispose();
            imgPhoto.Dispose();
            bmPhoto.Dispose();
            grPhoto.Dispose();

            return mm;
        }

        public static Image ResizeImage(Image srcImage, int newWidth, int newHeight)
        {
            using (Bitmap imagenBitmap = new Bitmap(newWidth, newHeight, PixelFormat.Format24bppRgb))
            {
                //imagenBitmap.SetResolution(Convert.ToInt32(srcImage.HorizontalResolution), Convert.ToInt32(srcImage.HorizontalResolution));
                imagenBitmap.SetResolution(Convert.ToInt32(srcImage.HorizontalResolution), Convert.ToInt32(srcImage.VerticalResolution));
                using (Graphics imagenGraphics = Graphics.FromImage(imagenBitmap))
                {
                    imagenGraphics.SmoothingMode = SmoothingMode.AntiAlias;
                    imagenGraphics.InterpolationMode = InterpolationMode.HighQualityBicubic;
                    imagenGraphics.PixelOffsetMode = PixelOffsetMode.HighQuality;
                    imagenGraphics.DrawImage(srcImage, new Rectangle(0, 0, newWidth, newHeight), new Rectangle(0, 0, srcImage.Width, srcImage.Height), GraphicsUnit.Pixel);
                    MemoryStream imagenMemoryStream = new MemoryStream();
                    imagenBitmap.Save(imagenMemoryStream, ImageFormat.Bmp);
                    srcImage = Image.FromStream(imagenMemoryStream);
                }
                imagenBitmap.Dispose();
            }
            return srcImage;
        }
    }
}
