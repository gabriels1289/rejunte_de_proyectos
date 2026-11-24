using System;
using System.Collections.Generic;
using System.Text;
using System.Security.Cryptography;
using System.IO;

namespace IRCASHDATAMODEL.Utiles
{
    /// <summary>
    /// Clase auxiliar para encriptar cadenas.
    /// </summary>
   public class Encriptacion
    {
        #region Variables y Constantes

        private const String myKey = "¿{IrOnLiNe}?"; // 24 bytes
        private const String myIV  = "<*P%@-u)+*>?"; // 24 bytes        
        #endregion

        #region Funciones

        public static String EncriptarMD5(String texto)
        {
            Byte[] input;
            Byte[] output;
            System.Security.Cryptography.MD5CryptoServiceProvider oMD5 = new System.Security.Cryptography.MD5CryptoServiceProvider();
            System.Text.ASCIIEncoding textoASCII = new ASCIIEncoding();
            input = textoASCII.GetBytes(texto);
            output = oMD5.ComputeHash(input);
            return (System.Convert.ToBase64String(output, 0, output.Length));
        }

        public static Byte[] EncriptarTripleDES(String texto)
        {
            return EncriptarTripleDES(texto, String.Empty, String.Empty);
        }

        public static Byte[] EncriptarTripleDES(String texto, String clave, String vector)
        {
            UnicodeEncoding encoder = new UnicodeEncoding();
            Byte[] key;
            Byte[] IV;
            Char[] cKey;
            Char[] cIV;

            if (clave == String.Empty || vector == String.Empty)
            {
                cKey = myKey.ToCharArray();
                cIV = myIV.ToCharArray();
                key = encoder.GetBytes(cKey);
                IV = encoder.GetBytes(cIV);
            }
            else
            {
                cKey = clave.ToCharArray();
                cIV = vector.ToCharArray();
                key = encoder.GetBytes(cKey);
                IV = encoder.GetBytes(cIV);
                if (!(key.Length == 24 && IV.Length == 24))
                {
                    throw new Exception("La clave y el vector de inicialización deben ser de 24 bytes cada uno.");
                }
            }

            return EncriptarTripleDES(texto, key, IV);
        }

        public static Byte[] EncriptarTripleDES(String texto, Byte[] key, Byte[] IV)
        {
            UnicodeEncoding encoder = new UnicodeEncoding();
            TripleDESCryptoServiceProvider myCrypto = new TripleDESCryptoServiceProvider();
            Byte[] toEncrypt;

            try
            {
                //Convierte los datos a un array de bytes.
                toEncrypt = encoder.GetBytes(texto);

                //Obtencion de la ionterface del encriptador
                ICryptoTransform encryptor = myCrypto.CreateEncryptor(key, IV);
                //Encripta los datos en un memory stream.
                MemoryStream msEncrypt = new MemoryStream();
                CryptoStream csEncrypt = new CryptoStream(msEncrypt, encryptor, CryptoStreamMode.Write);

                //Escribe los datos a un crypto-stream y le hace un flush.
                csEncrypt.Write(toEncrypt, 0, toEncrypt.Length);
                csEncrypt.FlushFinalBlock();
                msEncrypt.Position = 0;

                //obtiene el array de bytes encriptado.
                Byte[] result = new Byte[msEncrypt.Length]; //-1
                msEncrypt.Read(result, 0, (Int32)msEncrypt.Length);
                csEncrypt.Close();

                return result;
            }
            catch (Exception ex)
            {
                throw new Exception(ex.Message);
            }
        }

        public static String DesencriptarTripleDES(Byte[] bytesEncriptados)
        {
            return DesencriptarTripleDES(bytesEncriptados, String.Empty, String.Empty);
        }

        public static String DesencriptarTripleDES(Byte[] bytesEncriptados, String clave, String vector)
        {
            UnicodeEncoding encoder = new UnicodeEncoding();
            Byte[] key;
            Byte[] IV;
            Char[] cKey;
            Char[] cIV;

            if (clave == String.Empty || vector == String.Empty)
            {
                cKey = myKey.ToCharArray();
                cIV = myIV.ToCharArray();
                key = encoder.GetBytes(cKey);
                IV = encoder.GetBytes(cIV);
            }
            else
            {
                cKey = clave.ToCharArray();
                cIV = vector.ToCharArray();
                key = encoder.GetBytes(cKey);
                IV = encoder.GetBytes(cIV);
                if (!(key.Length == 24 && IV.Length == 24))
                    throw new Exception("La clave y el vector de inicialización deben ser de 24 bytes cada uno.");
            }

            return DesencriptarTripleDES(bytesEncriptados, key, IV);
        }

        public static String DesencriptarTripleDES(Byte[] bytesEncriptados, Byte[] key, Byte[] iv)
        {
            try
            {
                UnicodeEncoding encoder = new UnicodeEncoding();
                TripleDESCryptoServiceProvider myCrypto = new TripleDESCryptoServiceProvider();

                ICryptoTransform decryptor = myCrypto.CreateDecryptor(key, iv);

                //Desencripta el mensaje encriptado
                MemoryStream msDecrypt = new MemoryStream();
                CryptoStream csDecrypt = new CryptoStream(msDecrypt, decryptor, CryptoStreamMode.Write);

                //Extrae los datos del crypto-stream
                csDecrypt.Write(bytesEncriptados, 0, bytesEncriptados.Length);
                csDecrypt.FlushFinalBlock();
                msDecrypt.Position = 0;

                //Convierte el array de bytes a string
                Byte[] result = new Byte[(msDecrypt.Length)]; //- 1
                msDecrypt.Read(result, 0, (Int32)msDecrypt.Length);
                csDecrypt.Close();
                UnicodeEncoding myEncoder = new UnicodeEncoding();

                return myEncoder.GetString(result);
            }
            catch
            {
                return null;
            }

        }

        // Transforma un string (000) a Bytes
        static public Byte[] ConcatenarBytes(String cadena)
        {
            Byte[] parte = null;

            try
            {
                if ((cadena.Length % 2) == 0)
                {
                    parte = new Byte[(cadena.Length / 2)];

                    for (Int32 i = 0; i < cadena.Length; i += 2)
                    {
                        parte[(i / 2)] = (byte)Convert.ToByte(cadena.Substring(i, 2), 16);
                    }
                }
            }
            catch (Exception ex)
            {
                parte = null;
                throw new Exception(ex.Message);
            }

            return parte;
        }

        static public String ConcatenarBytes(Byte[] bytes)
        {
            String cadena = null;
            try
            {
                if (bytes.Length > 0)
                {
                    foreach (byte oByte in bytes)
                    {
                        cadena += String.Format("{0:x2}", oByte).ToUpper();
                    }
                }
            }
            catch (Exception ex)
            {
                cadena = null;
                throw new Exception(ex.Message);
            }

            return cadena;

        }

        #endregion

    }


}
