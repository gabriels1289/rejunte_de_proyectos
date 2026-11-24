#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <direct.h>

#include <windows.h>
#include <string>
#include <stdio.h>
#include <fcntl.h>
#include <math.h>

#define ERR_OK      0           // Sin Error
#define ERR_NAK     1           // Dispositivo envio un No Acknowlegment
#define ERR_CHK     2           // Error de checksum en la respuesta del dispositivo
#define ERR_ED      3           // Se recibio respuesta del dispositivo desconocida
#define ERR_FILE    4           // No se puede abrir/escribir archivo
#define ERR_NORX    5           // Dispositivo no responde

#define BINARIO     0           // Formato de archivo binario
#define TITEXT      1           // Formato de archivo TI texto

#define PASSW_DEFAULT   0       // Password por defecto 32 x 0xFF
#define PASSW_ARCHIVO   1       // Password obtenido de archivo

#define ACC_PROG        1       // Accion de programar
#define ACC_VERIF       2       // Accion de verificar
#define ACC_BORR        3       // Accion de borrar
#define ACC_LECT        4       // Accion de leer


#define ERRACC_COM      1       // Falla comunicacion
#define ERRACC_PSW      2       // Password incorrecto
#define ERRACC_FILE     3       // No se puede abrir archivo
#define ERRACC_DATAV    4       // Falla verificacion

#define SALVAR_ARCHIVO  1       // Salvar lectura en archivo.

#define ARCHIVO_VACIO           "VACIO"
#define ARCHIVO_GRABAR          "Codigo.txt"
#define ARCHIVO_LOG             "Log\\Log.txt"
#define ARCHIVO_SALVAR_BINARIO  "Lecturas\\Lectura.bin"
#define ARCHIVO_SALVAR_TITEXT   "Lecturas\\Lectura.txt"
#define ARCHIVO_SALVAR_MAPA     "Lecturas\\Lectura.map"

#define CARPETA_LECTURAS        "Lecturas"
#define CARPETA_LOG             "Log"


#define BSL_SYNC    0x80
#define DATA_ACK    0x90
#define DATA_NAK    0xA0
#define QUEUE_SIZE  512         // Tamaño del buffer

#define MAX_DATABYTES   250

HANDLE hComPort;                // Handle del puerto
DCB comDCB;                     // Opciones de control del puerto COM
COMSTAT comState;               // Informacion de estado del puerto COM
COMMTIMEOUTS timeouts;          // Timeouts para la comunicacion

char portName[6] = "COM1";
char archivo[250] = "";
int contadorProgressBar = 1;


// Definicion de variables globales

int CuentaRx;
BYTE ch;
DWORD NrTx;
DWORD NrRx;
BOOL OnLine = FALSE;
char Tecla;

char BSLversion [5] = "?";
char Dispositivo [5]= "?";

BYTE ImagenMem_Lectura   [65536];
BYTE ImagenMem_Archivo [65536];
BYTE ArregloCMDTx[260];                                                         // Trama de la transmicion.
BYTE ArregloCMDRx[260];                                                         // Trama de la respuesta.
int LongitudTramaTx;
int LongitudTramaRx;


namespace Programador {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::IO::Ports;
	using namespace System::Diagnostics;
	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//

			progressBar1->Visible=false;	// Display the ProgressBar control.
			progressBar1->Minimum = 1;	// Set Minimum to 1 to represent the first
			progressBar1->Maximum = 251;	// Set Maximum to the total number
			progressBar1->Value = 1;// Set the initial value of the ProgressBar.
			progressBar1->Step = 1;	 // Set the Step property to a value of 1
			label2->Visible = false;
			label2->Text = "";
			LoadPortName();
			
			if (cmBoxPuerto->Items->Count > 0)
            {
                //_serialPort.PortName = cmBoxPuerto.Items[0].ToString();
				//strcpy(portName, cmBoxPuerto->Items[0]);
				String^ aux = cmBoxPuerto->Items[0]->ToString();
				for(int i = 0; i<aux->Length; i++)
					portName[i] = aux[i];

		/*		portName[0] = aux[0];
				portName[1] = aux[3];
				Debug::Write(aux[0]);
				 
			     System::String ^ text = gcnew String("");
		*/

				//convert_string_to_char(aux, portName, aux->Length);
				/*Debug::Write("PortName: ");
				for(int j=0; j<5; j++)
					  text += (wchar_t) portName[j];
						Debug::Write(text);
				 listBoxConsola->Items->Add(portName[0]);
						listBoxConsola->Refresh();*/
            }
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
			
		}
	private: System::Windows::Forms::ListBox^  listBoxConsola;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  buttonAbrir;
	private: System::Windows::Forms::Button^  buttonProgramar;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::ComboBox^  cmBoxPuerto;
	private: System::Windows::Forms::OpenFileDialog^  openFD;
	private: String^ chosen_file;
	private: String^ tmp;
	private: System::Windows::Forms::ProgressBar^  progressBar1;
	private: System::Windows::Forms::Label^  label2;

	private: System::ComponentModel::IContainer^  components;

	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->listBoxConsola = (gcnew System::Windows::Forms::ListBox());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->buttonAbrir = (gcnew System::Windows::Forms::Button());
			this->buttonProgramar = (gcnew System::Windows::Forms::Button());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->cmBoxPuerto = (gcnew System::Windows::Forms::ComboBox());
			this->openFD = (gcnew System::Windows::Forms::OpenFileDialog());
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->SuspendLayout();
			// 
			// listBoxConsola
			// 
			this->listBoxConsola->FormattingEnabled = true;
			this->listBoxConsola->Location = System::Drawing::Point(13, 150);
			this->listBoxConsola->Name = L"listBoxConsola";
			this->listBoxConsola->Size = System::Drawing::Size(440, 329);
			this->listBoxConsola->TabIndex = 0;
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Controls->Add(this->buttonAbrir);
			this->groupBox1->Location = System::Drawing::Point(13, 13);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(440, 68);
			this->groupBox1->TabIndex = 1;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Archivo";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(122, 20);
			this->label1->MaximumSize = System::Drawing::Size(320, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(22, 13);
			this->label1->TabIndex = 1;
			this->label1->Text = L"C:\\";
			// 
			// buttonAbrir
			// 
			this->buttonAbrir->Location = System::Drawing::Point(6, 19);
			this->buttonAbrir->Name = L"buttonAbrir";
			this->buttonAbrir->Size = System::Drawing::Size(109, 28);
			this->buttonAbrir->TabIndex = 0;
			this->buttonAbrir->Text = L"Abrir Archivo";
			this->buttonAbrir->UseVisualStyleBackColor = true;
			this->buttonAbrir->Click += gcnew System::EventHandler(this, &Form1::buttonAbrir_Click);
			// 
			// buttonProgramar
			// 
			this->buttonProgramar->Location = System::Drawing::Point(347, 96);
			this->buttonProgramar->Name = L"buttonProgramar";
			this->buttonProgramar->Size = System::Drawing::Size(106, 41);
			this->buttonProgramar->TabIndex = 2;
			this->buttonProgramar->Text = L"Programar";
			this->buttonProgramar->UseVisualStyleBackColor = true;
			this->buttonProgramar->Click += gcnew System::EventHandler(this, &Form1::buttonProgramar_Click);
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->cmBoxPuerto);
			this->groupBox2->Location = System::Drawing::Point(13, 87);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(177, 57);
			this->groupBox2->TabIndex = 3;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Puerto";
			// 
			// cmBoxPuerto
			// 
			this->cmBoxPuerto->FormattingEnabled = true;
			this->cmBoxPuerto->Location = System::Drawing::Point(6, 20);
			this->cmBoxPuerto->Name = L"cmBoxPuerto";
			this->cmBoxPuerto->Size = System::Drawing::Size(150, 21);
			this->cmBoxPuerto->TabIndex = 0;
			this->cmBoxPuerto->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::cmBoxPuerto_SelectedIndexChanged);
			// 
			// progressBar1
			// 
			this->progressBar1->Location = System::Drawing::Point(196, 114);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(144, 23);
			this->progressBar1->TabIndex = 4;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(234, 98);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(40, 13);
			this->label2->TabIndex = 5;
			this->label2->Text = L"Accion";
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(465, 490);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->progressBar1);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->buttonProgramar);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->listBoxConsola);
			this->MaximizeBox = false;
			this->Name = L"Form1";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"BootStrap Loader - Ocean Servicios On-Line S.A.";
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	

	private: System::Void buttonAbrir_Click(System::Object^  sender, System::EventArgs^  e) {
				openFD->Filter = "TI-TEXT|*.txt";
				openFD->Title = "Abrir archivo";
				if(openFD->ShowDialog() == System::Windows::Forms::DialogResult::OK)
				{
					/*System::IO::StreamReader ^ sr = gcnew
					System::IO::StreamReader(openFD->FileName);
					MessageBox::Show(sr->ReadToEnd());
					sr->Close();*/
					label1->Text = openFD->FileName;
					chosen_file = openFD->FileName;
					for(int i=0 ; i<chosen_file->Length; i++)
						archivo[i] = chosen_file[i];
					//MessageBox::Show(chosen_file);
				}
			 }

private: System::Void cmBoxPuerto_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 if (cmBoxPuerto->SelectedIndex >= 0) // me fijo si hay puertos disponibles
                tmp = cmBoxPuerto->SelectedItem->ToString();
            else
                tmp = cmBoxPuerto->Items[0]->ToString(); // Primer puerto por defecto

			 for(int i = 0; i<tmp->Length; i++)
					portName[i] = tmp[i];
		 }
private: System::Void buttonProgramar_Click(System::Object^  sender, System::EventArgs^  e) {
		  //if (chosen_file->Length > 0)
			if( !String::IsNullOrEmpty(chosen_file) )
                {
                    if (cmBoxPuerto->SelectedIndex >= 0)
                    {
                        listBoxConsola->Items->Clear();
						cmBoxPuerto->Enabled = false;
						buttonAbrir->Enabled = false;
						buttonProgramar->Enabled = false;
						progressBar1->Visible=false;
						label2->Text = "";
						label2->Visible = false;
						label2->Refresh();
                        listBoxConsola->Items->Add(" ---------------------------------------------------------------------------------------------------------- ");
                        listBoxConsola->Items->Add(" PROGRAMADOR POR BOOTSTRAP LOADER DE MSP430 ");
                        listBoxConsola->Items->Add(" ---------------------------------------------------------------------------------------------------------- ");
						listBoxConsola->Items->Add("");
						listBoxConsola->Items->Add(" Configurando el puerto");
						listBoxConsola->Refresh();

						ConfigurarPuerto(); // Abre y configura el puerto
						
                        //  bool flag_mensaje_error = false;
                        label2->Visible = true;
						label2->Text = "Conectando...";
						label2->Refresh();
						while (Inicializacion() > 0)
                        {
                            //if (flag_mensaje_error == false)
                            //{
                            listBoxConsola->Items->Add(" -Hardware desconectado.");
                            listBoxConsola->Refresh();
							//   Debug::WriteLine(" -Hardware desconectado.");
                            //    flag_mensaje_error = true;
                            //}
                        }
						label2->Text = "Conectado";
						label2->Refresh();

						String ^ auxDispositivo = gcnew String("");
						for(int i= 0; Dispositivo[i]!='\0'; i++)
							auxDispositivo += (wchar_t)Dispositivo[i];
						String ^ auxBSLversion = gcnew String("");
						for(int i= 0; BSLversion[i]!='\0'; i++)
							auxBSLversion += (wchar_t)BSLversion[i];

                        listBoxConsola->Items->Add(" CONECTADO:");
						listBoxConsola->Items->Add("\tDispositivo conectado: \t" + auxDispositivo);
                        listBoxConsola->Items->Add("\tVersion del software: \t" + auxBSLversion);
						listBoxConsola->Items->Add("");
                        listBoxConsola->Refresh();

                       // Debug::WriteLine("CONECTADO:");

                        Changebaudrate();   // Se cambia la velocidad a 38400 BAUDIOS
										
						contadorProgressBar = 1;
						progressBar1->Visible = true;
						progressBar1->Refresh();
						//this->timer1->Start();
                        GrabacionRapida(archivo);
						//LecturaMemoria(0x0000, 0xFFFE, archivo, SALVAR_ARCHIVO);  // Llama funcion de lectura
                        Verificar(archivo);   // Llama funcion de verificacion
                        EjecutarPrograma();
						label2->Text = "";
						label2->Visible = false;
						label2->Refresh();
						//this->timer1->Stop();
						progressBar1->Value = 251;
						progressBar1->Refresh();
						Sleep(1000);	//Espera 2 segundos para que se vea la barra de progreso llena
						progressBar1->Value = 1;
						progressBar1->Visible = false;
						progressBar1->Refresh();
						contadorProgressBar = 1;
						cmBoxPuerto->Enabled = true;
						buttonAbrir->Enabled = true;
						buttonProgramar->Enabled = true;
                        CloseHandle(hComPort);
                    }
                    else MessageBox::Show("Primero seleccione un puerto \n Verifique que el dispositivo este conectado");
                }
                else MessageBox::Show("Primero debe seleccionar un archivo");
		 }

#pragma region funciones
		// Display Port values and prompt user to enter a port. 
		private: void LoadPortName()
        {
            array<String^>^ serialPorts = nullptr;
			try
			{
				// Get a list of serial port names.
				serialPorts = SerialPort::GetPortNames();
			}
			catch (Win32Exception^ ex)
			{
				MessageBox::Show(ex->Message);
			}

			//Debug::WriteLine("The following serial ports were found:");

			// Display each port name to the console. 
			for each(String^ port in serialPorts)
			{
				//Debug::WriteLine(port);
				cmBoxPuerto->Items->Add(port);
			}
        }


	//Abre el puerto y lo configura para la comunicacion
	void ConfigurarPuerto (void)
	{
	   //Apertura del puerto.
		//(LPCWSTR)
		//wchar_t * AportName =  convertCharArrayToLPCWSTR(portName);
	   hComPort= CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	
	   if (hComPort == INVALID_HANDLE_VALUE)
	   {
	        listBoxConsola->Items->Add("Error: No se puede abrir puerto. Puerto puede estar siendo usado por otro programa");
			 listBoxConsola->Refresh();
	       return;
	   }
	   if (SetupComm(hComPort, QUEUE_SIZE, QUEUE_SIZE) == 0)
	   {
	        listBoxConsola->Items->Add("Error: ");
			 listBoxConsola->Refresh();
	       return;
	   }
	
	   //Configuracion del puerto.
	   comDCB.BaudRate     = CBR_9600;                 // Velocidad 9600 baudios.
	   comDCB.ByteSize     = 8;
	   comDCB.Parity       = EVENPARITY;
	   comDCB.StopBits     = ONESTOPBIT;
	   comDCB.fBinary      = TRUE;                     // Transmision binaria activa.
	   comDCB.fParity      = TRUE;                     // Verificacion de paridad activada.
	   comDCB.fRtsControl  = RTS_CONTROL_ENABLE;       // Linea RTS ON.
	   comDCB.fDtrControl  = DTR_CONTROL_ENABLE;       // Linea DTR ON.
	
	   if (!SetCommState(hComPort, &comDCB))           // Confirmacion de parametros.
	   {
	        listBoxConsola->Items->Add("Error: No se puede configurar el puerto.");
			listBoxConsola->Refresh();
			return;
	   }
	
	   // Configuracion de timeouts.
	   timeouts.ReadIntervalTimeout            = 0;
	   timeouts.ReadTotalTimeoutMultiplier     = 0;
	   timeouts.ReadTotalTimeoutConstant       = 0;
	   timeouts.WriteTotalTimeoutMultiplier    = 0;
	   timeouts.WriteTotalTimeoutConstant      = 0;
	   if (!SetCommTimeouts(hComPort, &timeouts))
	   {
			listBoxConsola->Items->Add("Error: No se pueden establecer timeouts.");
			 listBoxConsola->Refresh();
			return;
	   }
	
	   // Inicializacion de buffers de transmicion y recepcion.
	   PurgeComm(hComPort, PURGE_TXCLEAR | PURGE_TXABORT);
	   PurgeComm(hComPort, PURGE_RXCLEAR | PURGE_RXABORT);
	}

	//Controla el pin RST/NMI del microcontrolador
	void SetRSTpin(BOOL level)
	{
	    if (level == TRUE)
	    comDCB.fDtrControl = DTR_CONTROL_ENABLE;
	    else
	    comDCB.fDtrControl = DTR_CONTROL_DISABLE;
	    SetCommState(hComPort, &comDCB);
	    return;
	}
	
	//Controla el pin TCK del microcontrolador
	void SetTCKpin(BOOL level)
	{
	    if (level == TRUE)
	    comDCB.fRtsControl = RTS_CONTROL_ENABLE;
	    else
	    comDCB.fRtsControl = RTS_CONTROL_DISABLE;
	    SetCommState(hComPort, &comDCB);
	    return;
	}

	// Resetea al micro en dos modos. 0: Ejecucion de codigo y 1: Entrada a BSL
	void ResetMicro(BOOL TipoReset)
	{
	   if (!TipoReset)
	   {
	       // Reset del microcontrolador (ejecucion)
	       SetRSTpin(0);           Sleep(10);
	       SetRSTpin(1);           Sleep(10);
	   }
	   else
	   {
	       // Reset del microcontrolador (entrada al BSL)
	       SetRSTpin(0);           Sleep(10);
	       SetTCKpin(1);           Sleep(10);
	       SetTCKpin(0);           Sleep(10);
	       SetTCKpin(1);           Sleep(10);
	       SetTCKpin(0);           Sleep(10);
	       SetTCKpin(1);           Sleep(10);
	       SetTCKpin(0);           Sleep(10);
	       SetRSTpin(1);           Sleep(10);
	       SetTCKpin(1);           Sleep(10);
	   }

	   Sleep(250);	// Retardo para que el oscilador entre en regimen.

	   PurgeComm(hComPort, PURGE_TXCLEAR | PURGE_TXABORT);
	   PurgeComm(hComPort, PURGE_RXCLEAR | PURGE_RXABORT);

	   return;
	}

	// Espera recibir una determinada cantidad de bytes en un tiempo determinado
	int EsperarDatos(int cantidad, DWORD timeout)
	{
	    DWORD errors;
	    int CuentaRx = 0;
	    DWORD TiempoInicio = GetTickCount();
	    DWORD TiempoActual;
	    do
	    {
	        ClearCommError(hComPort, &errors, &comState);
	        TiempoActual = GetTickCount();
	    } while (((CuentaRx = comState.cbInQue) < cantidad) && (TiempoActual - TiempoInicio) < timeout);
	
	    return(CuentaRx);
	}

	 // Envia el byte de sincronizacion y espera la respuesta.
	BOOL SincroBSL (void)
	{
	    PurgeComm(hComPort, PURGE_TXCLEAR | PURGE_TXABORT);
	    PurgeComm(hComPort, PURGE_RXCLEAR | PURGE_RXABORT);
	
	    ch = BSL_SYNC;
	    WriteFile(hComPort, &ch, 1, &NrTx, NULL);           // Envia BSL_SYNC 0x80
	
	    Sleep(10);
	
	    CuentaRx = EsperarDatos(1, 1000);                    // Espera 1 byte en 100 mS
	    if (CuentaRx > 0)
	    {
	        ReadFile(hComPort, &ch, 1, &NrRx, NULL);
	        if (ch == DATA_ACK)
	        {
	            //printf("Comunicacion establecida se recibio hexa = %x \n",ch);
	            return (0);
	        }
	        else
	        return 1;        // Error no se recibio el byte esperado (0x90)
	    }
	    else
	    return 1;            // Error no se recibio ningun dato
	}

	// Calcula parte de baja de un Checksum de 16 bits (CKL).
	BYTE CalcularCKL (BYTE Trama[],int LongitudTrama)
	{
	    int i;
	    BYTE CKL = 0x00;
	    for (i=0;i<=LongitudTrama-4;i=i+2)
	        CKL = CKL^Trama[i];
	    CKL = ~CKL;
	    return (CKL);
	}
	
	// Calcula parte de alta de un Checksum de 16 bits (CKH).
	BYTE CalcularCKH (BYTE Trama[],int LongitudTrama)
	{
	    int i;
	    BYTE CKH = 0x00;
	    for (i=1;i<=LongitudTrama-3;i=i+2)
	        CKH = CKH^Trama[i];
	    CKH = ~CKH;
	    return (CKH);
	}

	// Calcula el checksum de una trama y lo compara con el propio que contiene la trama
	int VerificarChecksum(BYTE Trama[],int LongitudTrama)
	{
	    if ((CalcularCKL(Trama, LongitudTrama) == Trama[LongitudTrama-2]) && (CalcularCKH(Trama, LongitudTrama) == Trama[LongitudTrama-1]) )
	        return 1;                   // Checksum OK
	    else
	        return 0;                   // Fallo verificacion de Checksum.
	}

	int Inicializacion()
	{
	    ResetMicro(1);                                                      // Reset para entrar al BSL
	    if (!TXBSLversion()) return ERR_OK;
	    else return ERR_NORX;
	}

	int Changebaudrate()
	{
	    int Resultado;
	
	    LongitudTramaTx = 10;
	    LongitudTramaRx = 1;
	
	    ArregloCMDTx[0] = 0x80;
	    ArregloCMDTx[1] = 0x20;
	    ArregloCMDTx[2] = 0x04;
	    ArregloCMDTx[3] = 0x04;
	    ArregloCMDTx[4] = 0xE0;
	    ArregloCMDTx[5] = 0x87;
	    ArregloCMDTx[6] = 0x02;
	    ArregloCMDTx[7] = 0x00;
	    ArregloCMDTx[8] = CalcularCKL(ArregloCMDTx,LongitudTramaTx);
	    ArregloCMDTx[9] = CalcularCKH(ArregloCMDTx,LongitudTramaTx);
	
	    if (Resultado = EnviarRecibir() == ERR_OK)
	    {
	        listBoxConsola->Items->Add(" -Velocidad: 38400 baudios");
			listBoxConsola->Refresh();
	        Sleep(1000);
	        comDCB.BaudRate = CBR_38400;                 // Velocidad 38400 baudios.
	
	        if (!SetCommState(hComPort, &comDCB))           // Confirmacion de parametros.
	        {
	            listBoxConsola->Items->Add("Error: No se puede configurar el puerto.");
				listBoxConsola->Refresh();
	            //exit(1);
	        }
	        return ERR_OK;
	    }
		else{
			listBoxConsola->Items->Add(" -Velocidad: 9600 baudios");
			listBoxConsola->Refresh();
		}
	    return ERR_NAK;
	}

	// Comando Mass erase
	// Borra completamente la memoria flash.
	int Masserase(void)
	{
	    ArregloCMDTx[0] = 0x80;
	    ArregloCMDTx[1] = 0x18;
	    ArregloCMDTx[2] = 0x04;
	    ArregloCMDTx[3] = 0x04;
	    ArregloCMDTx[4] = 0x00;
	    ArregloCMDTx[5] = 0x00;
	    ArregloCMDTx[6] = 0x06;
	    ArregloCMDTx[7] = 0xA5;
	    ArregloCMDTx[8] = 0x7D;
	    ArregloCMDTx[9] = 0x46;
	
	    LongitudTramaTx = 10;
	    LongitudTramaRx = 1;
	
	    return EnviarRecibir();
	}

	int CrearImagenArchivo(char NombreArchivo[],int FormatoArchivo)
	{
	
	    FILE *pFile;
	    int PesoArchivo;
	    WORD pImagenMem_Archivo;
	    int i;
	    char Caracter;
	    BYTE DireccionAlta, DireccionBaja;
	
	    for (i = 0; i < 0x10000; ++i)                     // Inicializacion de Imagen de memoria a leer.
	        ImagenMem_Archivo[i] = 0xFF;
	
	
	    pFile = fopen(NombreArchivo,"r");               //  Abrir archivo
	    if (pFile == NULL)
	    {
	        listBoxConsola->Items->Add("No se puede abrir archivo.");
			listBoxConsola->Refresh();
	        return ERR_FILE;
	    }
	
	
	    if (FormatoArchivo == TITEXT)                   // Formato de archivo TI-TEXT.
	    {
	        do                                          // Recorrer archivo y extraer informacion.
	        {
	            Caracter = fgetc(pFile);
	
	            if (Caracter == '@')                                                                    // se encuentra '@'
	            {
	                DireccionAlta = ObtenerByte(pFile);                                                 // Obtiene direccion
	                DireccionBaja = ObtenerByte(pFile);
	                pImagenMem_Archivo = DireccionAlta * 256 + DireccionBaja;                           // Carga puntero
	
	            }
	            else if (Caracter != '\n' && Caracter != ' ' && Caracter != 'q')
	            {
	                fseek(pFile,-1,SEEK_CUR);                                                           // se vuelve un paso atras
	                ImagenMem_Archivo[pImagenMem_Archivo] = ObtenerByte(pFile) ;                        // se lee y se procesa.
	                ++pImagenMem_Archivo;                                                               // se incrementa el puntero de la imgagen de archivo
	            }
	
	        }while (Caracter != 'q');
	
	    }
	
	    else if (FormatoArchivo == BINARIO)
	    {
	        return ERR_FILE;
	    }
	
	    fclose(pFile);
	
	    return ERR_OK;
	}

	// Comando RX password
	// Lee el archivo password.txt y le envia al dispositivo el password correspondiente
	int RXpassword (BOOL PasswordPorDefecto, char NombreArchivo[])
	{
	    int i;
	
	    ArregloCMDTx[0] = 0x80;
	    ArregloCMDTx[1] = 0x10;
	    ArregloCMDTx[2] = 0x24;
	    ArregloCMDTx[3] = 0x24;
	    ArregloCMDTx[4] = 0xE0;
	    ArregloCMDTx[5] = 0xFF;
	    ArregloCMDTx[6] = 0x20;
	    ArregloCMDTx[7] = 0x00;
	
	    LongitudTramaTx = 42;
	    LongitudTramaRx = 1;
	
	    FILE *pFile;
	
	    // Se carga el archivo con el password
	    if (PasswordPorDefecto == PASSW_DEFAULT)
	    {
	        for (i = 8; i <= 39; ++i)                                   // Se usa password por defecto (32 bytes 0xFF)
	            ArregloCMDTx[i] = 0xFF;
	    }
	
	    else if (PasswordPorDefecto == PASSW_ARCHIVO)
	    {
	        CrearImagenArchivo(NombreArchivo, TITEXT);                  // Se abre archivo y se crea imagen
	        for (i = 8; i <= 39; ++i)                                   // Se extrae password de los ultimos 32 bytes
	            ArregloCMDTx[i] = ImagenMem_Archivo[0xFFE0 + i - 8];
	
	    }
	
	    ArregloCMDTx[40] = CalcularCKL(ArregloCMDTx,LongitudTramaTx);
	    ArregloCMDTx[41] = CalcularCKH(ArregloCMDTx,LongitudTramaTx);
	
	    return EnviarRecibir();
	}

	// Lee 2 bytes consecutivos en el archivo y devuelve un BYTE
	BYTE ObtenerByte(FILE *pFile)
	{
	    char Caract[2];
	    BYTE Byte;
	    Caract[0] = fgetc(pFile);
	    Caract[1] = fgetc(pFile);
	    sscanf(Caract,"%2X",&Byte);
	    return Byte;
	}

	// Arma la trama del comando RX data block e invoca la funcion EnviarRecibir()
	int ArmarTrama(BYTE TramaDataBytes[],BYTE AH,BYTE AL,int Longitud,int LongitudRx)
	{
		int i;
	
	    for (i=0;i<260;++i)
	        ArregloCMDTx[i] = 0x00;
	
	   // printf ("\nDATA: \n");
	
		ArregloCMDTx[0] = 0x80;                            // HDR
		ArregloCMDTx[1] = 0x12;                            // CMD
		ArregloCMDTx[2] = Longitud+4;                      // L1
		ArregloCMDTx[3] = Longitud+4;                      // L2
		ArregloCMDTx[4] = AL;                              // AL
		ArregloCMDTx[5] = AH;                              // AH
		ArregloCMDTx[6] = Longitud;                        // LL
		ArregloCMDTx[7] = 0x00;                            // LH
	
		for(i=8;i<8+Longitud;++i)                           // D1...Dn
		ArregloCMDTx[i] = TramaDataBytes[i-8];
	
		if (Longitud % 2 != 0)                              // Cantidad par de bytes?   SI :: Agrega 1 byte 0xFF.
	    {                                                   //                          NO :: Queda como esta.
	        ++Longitud;
	        ArregloCMDTx[2] = Longitud+4;                  // L1
	        ArregloCMDTx[3] = Longitud+4;                  // L2
	        ArregloCMDTx[6] = Longitud;                    // LL
	        ArregloCMDTx[i] = 0xFF;                        // Agregado
	        ++i;
	    }
	
	    LongitudTramaTx = Longitud + 10;
	
		ArregloCMDTx[i]   = CalcularCKL(ArregloCMDTx,LongitudTramaTx);
		ArregloCMDTx[++i] = CalcularCKH(ArregloCMDTx,LongitudTramaTx);
	
	
		/*for(i=0;i<10+Longitud;++i)
		printf ("%X\t", ArregloCMDTx[i]);
	
		printf ("\n\n --------------FIN TRAMA------------- i = %X\n\n ",i);*/
	
	
		return EnviarRecibir();
	}

	// Envia la trama TramaTx y espera recibir en TramaRx
	int EnviarRecibir()
	{
	    int i;
	
	    PurgeComm(hComPort, PURGE_TXCLEAR | PURGE_TXABORT);
	    PurgeComm(hComPort, PURGE_RXCLEAR | PURGE_RXABORT);
	    if (SincroBSL())                                                    // Sincronizacion
	    {
	        //printf("Error\n");
	        return ERR_NAK;
	    }
	
	    for (i = 0; i < LongitudTramaTx; ++i)
	        WriteFile(hComPort, &ArregloCMDTx[i], 1, &NrTx, NULL);
	
	    CuentaRx = EsperarDatos(LongitudTramaRx, 1000);                     // Espera 22 bytes en 100 mS
	
	    if (CuentaRx == LongitudTramaRx)                                    // Se recibio la cantida esperada de bytes?
	    {
	        if (CuentaRx == 0) return ERR_NORX;                             // Se recibio 0 byte (Dispositivo no responde)
	        else if (CuentaRx == 1)                                         // Se recibio 1 byte (ACK o NAK)
	        {
	            ReadFile(hComPort, &ch, 1, &NrRx, NULL);
	            switch (ch)
	            {
	                case DATA_ACK:
	                    return ERR_OK;
	                    break;
	                case DATA_NAK:
	                    return ERR_NAK;
	                    break;
	            }
	
	        }
	        else                                                            // Se recibieron mas bytes.
	        {
	            for (i=0; i<CuentaRx; ++i)
	            {
	                ReadFile(hComPort, &ch, 1, &NrRx, NULL);
	                ArregloCMDRx[i] = ch;
	            }
	            if (!VerificarChecksum(ArregloCMDRx,CuentaRx))          // Se verifica el checksum de los datos recibidos
	            {
	                listBoxConsola->Items->Add(" -Error en checksum de datos recibidos");    // Checksum incorrecto.
					listBoxConsola->Refresh();
					return ERR_CHK;
	            }
	            else return ERR_OK;
	        }
	    }
	    else return ERR_ED;                                                 // Se recibieron una cantidad distinta de la esperada.
	}

	// Comando TX BSL version
	// Entrega informacion sobre el chip y la version del software del BSL.
	int TXBSLversion (void)
	{
	    int Resultado;
	
	    ArregloCMDTx[0] = 0x80;
	    ArregloCMDTx[1] = 0x1E;
	    ArregloCMDTx[2] = 0x04;
	    ArregloCMDTx[3] = 0x04;
	    ArregloCMDTx[4] = 0x00;
	    ArregloCMDTx[5] = 0x00;
	    ArregloCMDTx[6] = 0x00;
	    ArregloCMDTx[7] = 0x00;
	    ArregloCMDTx[8] = 0x7B;
	    ArregloCMDTx[9] = 0xE5;
	
	    LongitudTramaTx = 10;
	    LongitudTramaRx = 22;
	
	    Resultado = EnviarRecibir();
	
	    if (Resultado == 0)
	    {
	            sprintf(BSLversion,"%x.%x",ArregloCMDRx[14],ArregloCMDRx[15]);
	            sprintf(Dispositivo,"%X%X",ArregloCMDRx[4],ArregloCMDRx[5]);
	            return ERR_OK;
	    }
	    else return Resultado;
	}

	// Comando RX data block
	// Graba en el  microcontrolador el contenido del archivo de formato TI-TEXT.
	int RXdatablock(char NombreArchivo[])
	{
	    FILE *pFile;
	    BYTE Caracter,DireccionBaja,DireccionAlta;
	    BOOL TramaAbierta = FALSE;
	    BYTE TramaDataBytes[MAX_DATABYTES];
	
	    int i = 0;
	    int Resultado = 0;
	
	    pFile = fopen(NombreArchivo,"r");
	
	    if(pFile==NULL)
	    {
	        listBoxConsola->Items->Add(" -Error al intentar abrir archivo.");
	        listBoxConsola->Refresh();
			return (ERR_FILE);
	    }
	
	    listBoxConsola->Items->Add(" -Programando. Por favor aguarde...");
		listBoxConsola->Refresh();
	
	    do
	    {
	        Caracter = fgetc(pFile);
	
	        if (Caracter == '@')
	        {
	            if (!TramaAbierta)					                                                //Trama cerrada
	            {
	
	                DireccionAlta = ObtenerByte(pFile);
	                DireccionBaja = ObtenerByte(pFile);
	
	                TramaAbierta = TRUE;
	            }
	                else                                                                            //Trama abierta
	            {
	                if (Resultado += ArmarTrama(TramaDataBytes,DireccionAlta,DireccionBaja,i,1)) return ERR_NAK;        //Se finaliza la trama que se estaba leyendo del archivo
	                fseek(pFile,-1,SEEK_CUR);                                                       //Se vuelve un caracter atras.
	                TramaAbierta = FALSE;                                                           //Se desactiva el flag de trama abierta.
	                i = 0;
	            }
	        }
	        else if (Caracter != '\n' && Caracter != ' ' && Caracter != 'q')
	        {
	            fseek(pFile,-1,SEEK_CUR);                                                           // se vuelve un paso atras
	            TramaDataBytes[i]=ObtenerByte(pFile);                                               // se lee y se procesa.
	            ++i;                                                                                // se incrementa contador de bytes de datos
	            if (i == MAX_DATABYTES)                                                                         // Si se alcanza los 250 bytes de datos
	            {
	                if (Resultado += ArmarTrama(TramaDataBytes,DireccionAlta,DireccionBaja,i,1)) return ERR_NAK;        // se arama la trama
	                TramaAbierta = FALSE;                                                           // se borra el flag
	                DireccionBaja = DireccionBaja + i;
	                if (DireccionBaja < i) ++DireccionAlta;
	                i = 0;
	                TramaAbierta = TRUE;

					if(contadorProgressBar == 250)
						contadorProgressBar = 1;
					else
						contadorProgressBar++;
					progressBar1->Value = contadorProgressBar;
					progressBar1->Refresh();
	            }
				
	        }
	        else if (Caracter == 'q')
	        {
	            if (Resultado += ArmarTrama(TramaDataBytes,DireccionAlta,DireccionBaja,i,1)) return ERR_NAK;            // Se finaliza la trama
	            TramaAbierta = FALSE;                                                               // y se borra el flag.
				progressBar1->Value = 250;
				progressBar1->Refresh();
	        }
	
	    }while (Caracter != 'q');
	
	    fclose (pFile);
	
	    if (Resultado == 0)
	        return ERR_OK;
	    else return ERR_NAK;
	}

	void GrabacionRapida (char NombreArchivo[])
	{
	    int Resultado;
	    FILE *pFile;
	    char NombreArchivoLocal[256];

		listBoxConsola->Items->Add(" GRABACION");
		listBoxConsola->Refresh();
	
	    //if (!strcmp(NombreArchivo,ARCHIVO_VACIO))                               // Viene sin archivo designado
	    //    do
	    //    {
	    //        //Debug::WriteLine(" -Archivo a grabar: ");
	    //        //scanf ("%s", NombreArchivoLocal);
	    //        pFile = fopen(NombreArchivoLocal,"r");
	    //        if (!pFile) Debug::WriteLine(" -No se puede abrir archivo.\n");
	    //    }
	    //    while (!pFile);
	    //else strcpy (NombreArchivoLocal,NombreArchivo);                             // Viene con archivo designado
		
		strcpy (NombreArchivoLocal,NombreArchivo);                             // Viene con archivo designado
        pFile = fopen(NombreArchivoLocal,"r");
        if (!pFile)
		{
			listBoxConsola->Items->Add(" -No se puede abrir archivo.");
			listBoxConsola->Refresh();
			return;
		}
	
	
	    if (!Masserase())                                                           // Borado de memoria
	    {
			label2->Text = "Grabando...";
			label2->Refresh();
	        listBoxConsola->Items->Add(" -Borrado completo de memoria flash.");                      // Borrado OK!
			listBoxConsola->Refresh();
	        if (!RXpassword(PASSW_DEFAULT,NULL))                                    // Acceso con password
	        {
	            listBoxConsola->Items->Add(" -Password aceptado");                                   // Password OK!
				listBoxConsola->Refresh();
	            if (!RXdatablock(NombreArchivoLocal))                                    // Grabacion de datos
	            {
	                listBoxConsola->Items->Add(" -Se grabo correctamente");                         // Grabacion OK!
					listBoxConsola->Refresh();
	                Resultado = ERR_OK;
	            }
	            else
	            {
	                listBoxConsola->Items->Add(" -Error en la grabacion");                            // ERROR: Grabacion
					listBoxConsola->Refresh();
	                Resultado = ERRACC_COM;
	            }
	        }
	        else
	        {
	            listBoxConsola->Items->Add(" -Password incorrecto");                                 // ERROR: Password
				listBoxConsola->Refresh();
	            Resultado = ERRACC_PSW;
	        }
	    }
	    else
	    {
	        listBoxConsola->Items->Add(" -No se pudo borrar memoria flash.");                       // ERROR: Comunicacion
			listBoxConsola->Refresh();
	        Resultado = ERRACC_COM;
	        ResetMicro(1);                                                       // Vuelve a entrar modo BSL
	    }
	
	    CrearLog(ACC_PROG, NombreArchivoLocal, Resultado, NULL, NULL);
	}
	
	void CrearLog(int Accion, char NombreArchivo[], int Error, WORD Tabla[],DWORD TamanoTabla)
	{
	    FILE *pFileLog;
	    SYSTEMTIME lt;
	    char NombreAccion[64], DescripcionError[64], ResultadoAccion[64], NombreArchivoLocal[256];
	    int i;
	
	    if (Accion == ACC_PROG)
	        strcpy (NombreAccion, "Programacion rapida");
	    else if (Accion == ACC_VERIF)
	        strcpy (NombreAccion, "Verificacion");
	    else if (Accion == ACC_BORR)
	        strcpy (NombreAccion, "Borrado");
	    else if (Accion == ACC_LECT)
	        strcpy (NombreAccion, "Lectura");
	
	    if (Error == ERR_OK)
	        strcpy (DescripcionError, "-");
	    else if (Error == ERRACC_COM)
	        strcpy (DescripcionError, "Error en la comunicacion. (ERR 1)");
	    else if (Error == ERRACC_PSW)
	        strcpy (DescripcionError, "No reconoce password. Acceso denegado a memoria. (ERR 2)");
	    else if (Error == ERRACC_FILE)
	        strcpy (DescripcionError, "No se puede abrir archivo. (ERR 3)");
	    else if (Error == ERRACC_DATAV)
	        strcpy (DescripcionError, "Discrepancia de datos (ERR 4)");
	
	    if (!Error) strcpy (ResultadoAccion, "OK");
	    else strcpy (ResultadoAccion, "ERROR");
	
	    if (NombreArchivo == NULL) strcpy (NombreArchivoLocal, "-");
	    else strcpy(NombreArchivoLocal, NombreArchivo);
	
	    _mkdir(CARPETA_LOG);
	    pFileLog = fopen (ARCHIVO_LOG,"w");
	
	    GetLocalTime(&lt);
	
	    fprintf(pFileLog, "==========================================================================\n");
	    fprintf(pFileLog, "/* Archivo log con informacion de la ultima accion en BSLProg */          \n");
	    fprintf(pFileLog, "==========================================================================\n");
	    fprintf(pFileLog, "FECHA\t\t%04d/%02d/%02d:%02d:%02d:%02d\n",lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond);
	    fprintf(pFileLog, "ACCION\t\t%s\n",NombreAccion);
	    fprintf(pFileLog, "ARCHIVO\t\t%s\n",NombreArchivoLocal);
	    fprintf(pFileLog, "RESULTADO\t%s\n",ResultadoAccion);
	    fprintf(pFileLog, "ERROR\t\t%s\n",DescripcionError);
	
	    if(TamanoTabla)
	    {
	        fprintf(pFileLog, "\nDIRECCION\tESPERADO\tENCONTRADO\n------------------------------------------\n");
	        for (i = 0; i < TamanoTabla; ++i)
	            fprintf(pFileLog, "0x%X\t\t0x%X\t\t0x%X\n",Tabla[i], ImagenMem_Archivo [Tabla[i]], ImagenMem_Lectura [Tabla[i]]);
	        fprintf(pFileLog, "\n Se encontraron (%d) lugares de memoria corrompidos\n", TamanoTabla);
	    }
	    fprintf(pFileLog, "==========================================================================\n");
	
	    fclose(pFileLog);
	
	    return;
	}

	// Comando TX data block
	// Lee la memoria Flash / RAM o registros de control de modulos de 0x0000 a 0x01FF
	int TXdatablock(WORD DireccionInicio, WORD DireccionFinal)
	{
	    int i, j;
	
	    WORD n = DireccionFinal - DireccionInicio;
	
	    for(i=0; i<256; ++i)                                                           // Borra ArregloCMDRx
	    ArregloCMDRx[i] = 0;
	
	    LongitudTramaTx = 10;
	    LongitudTramaRx = n + 6;
	
	    ArregloCMDTx[0] = 0x80;
	    ArregloCMDTx[1] = 0x14;
	    ArregloCMDTx[2] = 0x04;
	    ArregloCMDTx[3] = 0x04;
	    ArregloCMDTx[4] = DireccionInicio;
	    ArregloCMDTx[5] = DireccionInicio >> 8;
	    ArregloCMDTx[6] = n;
	    ArregloCMDTx[7] = 0x00;
	    ArregloCMDTx[8] = CalcularCKL(ArregloCMDTx,LongitudTramaTx);
	    ArregloCMDTx[9] = CalcularCKH(ArregloCMDTx,LongitudTramaTx);
	
		if(contadorProgressBar == 250)
			contadorProgressBar = 1;
		else
			contadorProgressBar++;
		progressBar1->Value = contadorProgressBar;
		progressBar1->Refresh();

	    return EnviarRecibir();
	}

	int LecturaMemoria(WORD DirInicio_Lectura, WORD DirFinal_Lectura, char NombreArchivo[], BOOL SalvarArchivo)
	{
	    WORD DirAux_Lectura;
	    WORD BytesRestantes;
	    WORD OffsetLectura;
	    FILE *pFile, *pFileBin, *pFileTiText, *pFileMap;
	    char NombreArchivoLocal[256];
	
	    int i, j, Resultado = 0;
	
	    for (i = 0; i < 0x1000; ++i)                                                        // Inicializacion de Imagen de memoria a leer.
	        ImagenMem_Lectura[i] = 0xFF;
	
	    listBoxConsola->Items->Add(" LECTURA DE MEMORIA");
		listBoxConsola->Refresh();
	
	    //if (!strcmp(NombreArchivo,ARCHIVO_VACIO))                                           // Viene sin archivo designado
	    //    do
	    //    {
	    //        listBoxConsola->Items->Add(" -Archivo password: ");
	    //        scanf ("%s", NombreArchivoLocal);
	    //        pFile = fopen(NombreArchivoLocal,"r");
	    //        if (!pFile) listBoxConsola->Items->Add(" -No se puede abrir archivo.\n");
	    //    }
	    //    while (!pFile);
	    //else strcpy (NombreArchivoLocal,NombreArchivo);                                     // Viene con archivo designado

		strcpy (NombreArchivoLocal,NombreArchivo);                             // Viene con archivo designado
        pFile = fopen(NombreArchivoLocal,"r");
        if (!pFile)
		{
			listBoxConsola->Items->Add(" -No se puede abrir archivo.");
			listBoxConsola->Refresh();
			return 5;	//Agregue yo el retorno
		}
	
	    if (!RXpassword(PASSW_ARCHIVO, NombreArchivoLocal))                                      // Acceso a comandos protegidos
	    {
			//label2->Text = "Leyendo...";
			//label2->Refresh();
	        listBoxConsola->Items->Add(" -Password aceptado");
			listBoxConsola->Refresh();
	
	        DirAux_Lectura = DirInicio_Lectura;                                             // Inicializacion del registro auxiliar de direccion.
	        BytesRestantes = DirFinal_Lectura - DirInicio_Lectura;                          // Inicializacion del registro de bytes restantes por ser leidos.
	
	        listBoxConsola->Items->Add(" -Leyendo. Por favor aguarde...");
			listBoxConsola->Refresh();
	
	        do                                                                              // Ciclo de multiples lecturas simples
	        {
	            if (BytesRestantes < MAX_DATABYTES) OffsetLectura = BytesRestantes;         // Restan leer menos de 250 bytes
	            else                                OffsetLectura = MAX_DATABYTES;          // Restan leer mas de 250 bytes
	
	            if (TXdatablock(DirAux_Lectura, DirAux_Lectura + OffsetLectura + 2))        // Se invoca comando de lectura simple (250 bytes)
	            {
	                listBoxConsola->Items->Add(" -Error en la lectura de memoria.");
					listBoxConsola->Items->Add(DirAux_Lectura);
					listBoxConsola->Refresh();
	                BytesRestantes = OffsetLectura;                                                     // Se fuerza salida del bucle por error.
	                Resultado = ERRACC_COM;
	            }
	            else Resultado += ERR_OK;                                                   // Si no hay errores, Resultado se mantiene en 0.
	
	            //printf (".");
	
	            for (j = 0; j < OffsetLectura + 2; ++j)
	                ImagenMem_Lectura [DirAux_Lectura + j] =  ArregloCMDRx [j + 4];         // Crea Imagen
	
	            DirAux_Lectura = DirAux_Lectura + OffsetLectura;                            // Proxima Direccion inicial.
	            BytesRestantes -= OffsetLectura;                                            // Actualiza bytes que restan leer.
	            //printf ("%d ", BytesRestantes);
	
	        } while (BytesRestantes > 0);                                                   // Comprueba si quedan bytes por leer.
	
	        if (!Resultado) {
				listBoxConsola->Items->Add(" -Lectura OK");
				listBoxConsola->Refresh();
			}
	
	
	        if (SalvarArchivo)                                                              // Si se configuro salvar lectura en archivos
	        {
	            mkdir (CARPETA_LECTURAS);
	
	            // Salva archivo BINARIO
	            pFileBin = fopen(ARCHIVO_SALVAR_BINARIO, "w");
	            fwrite (ImagenMem_Lectura, 1 , sizeof(ImagenMem_Lectura) , pFileBin );
	            fclose (pFileBin);
	
	            // Salva archivo TI-TEXT
	            pFileTiText = fopen(ARCHIVO_SALVAR_TITEXT, "w");
	            fprintf (pFileTiText, "@0000\n");
	            for (i = 0; i < 0x10000; i = i + 16)
	            {
	                for (j = 0; j < 16; ++j)
	                    fprintf (pFileTiText, "%02.2X ",ImagenMem_Lectura[i + j]);
	                fprintf (pFileTiText, "\n");
	            }
	            fprintf (pFileTiText, "q\n");
	            fclose (pFileTiText);
	
	            // Salva MAPA de memoria
	            pFileMap = fopen(ARCHIVO_SALVAR_MAPA, "w");
	            for (i = 0; i < 0x10000; i = i + 16)
	            {
	                fprintf (pFileMap, "0x%04.4X\t",i);
	                for (j = 0; j < 16; ++j)
	                    fprintf (pFileMap, "%02.2X ", ImagenMem_Lectura[i + j]);
	                fprintf (pFileMap, "\t");
	                for (j = 0; j < 16; ++j)
	                {
	                    if (ImagenMem_Lectura[i + j] < 0x20) fprintf (pFileMap, ".");
	                    else fprintf (pFileMap, "%c", ImagenMem_Lectura[i + j]);
	                }
	                fprintf (pFileMap, "\n");
	            }
	            fclose (pFileMap);
	            listBoxConsola->Items->Add(" -Archivos guardados.");
				listBoxConsola->Refresh();
	        }
	    }	
	    else
	    {
	        listBoxConsola->Items->Add(" -Password incorrecto");
			listBoxConsola->Refresh();
	        Resultado = ERRACC_PSW;
	    }
	
	
	    CrearLog(ACC_LECT, NombreArchivoLocal, Resultado, NULL, NULL);
	
	    return Resultado;
	}

	void Verificar(char NombreArchivo[]) //int
	{
	    DWORD i, Resultado;
	    WORD DireccionErrores[0x10000];
	    WORD CantidadErrores = 0;
	    FILE *pFile;
	    char NombreArchivoLocal[256];
	
	    listBoxConsola->Items->Add(" VERIFICACION");
		listBoxConsola->Refresh();
		contadorProgressBar = 1;
		progressBar1->Value = contadorProgressBar;
		progressBar1->Refresh();
	
	   // if (!strcmp(NombreArchivo,ARCHIVO_VACIO))                               // Viene sin archivo designado
	   //     do
	   //     {
	   //         listBoxConsola->Items->Add(" -Archivo a comparar: ");
				//listBoxConsola->Refresh();
	   //         scanf ("%s", NombreArchivoLocal);
	   //         pFile = fopen(NombreArchivoLocal,"r");
	   //         if (!pFile) 
				//{
				//	listBoxConsola->Items->Add(" -No se puede abrir archivo.");
				//	listBoxConsola->Refresh();
				//}
	   //     }
	   //     while (!pFile);
	   // else strcpy (NombreArchivoLocal,NombreArchivo);                             // Viene con archivo designado

		strcpy (NombreArchivoLocal,NombreArchivo);                             // Viene con archivo designado
		pFile = fopen(NombreArchivoLocal,"r");
	    if (!pFile) 
		{
			listBoxConsola->Items->Add(" -No se puede abrir archivo.");
			listBoxConsola->Refresh();
			return;
		}


	    if (!CrearImagenArchivo(NombreArchivoLocal, TITEXT))             // Se abre archivo y se obtiene una imagen del archivo
	    {
				label2->Text = "Verificando...";
				label2->Refresh();
	            if(!(Resultado = LecturaMemoria(0x1000, 0xFFFE, NombreArchivoLocal, NULL)))  // Se realiza lectura de la memoria (Info + Main = 0x1000 a 0xFFFF)
	            {
	                for (i = 0x1000; i <= 0xFFFF; ++i)              // Comparacion de las dos imagenes (archivo y memoria del microntrolador)
	                {
	                    if (ImagenMem_Archivo [i] != ImagenMem_Lectura [i])
	                    {
	                        DireccionErrores[CantidadErrores] = i;  // Se guardan la direcciones donde hay discrepancia.
	                        ++ CantidadErrores;                     // Se incrementa contador de discrepancia.
	                    }
	                }
	
	                if (!CantidadErrores)
	                {
	                    listBoxConsola->Items->Add(" -Verificacion Completa");
						listBoxConsola->Refresh();
	                    Resultado = ERR_OK;
	                }
	                else
	                {
	                    listBoxConsola->Items->Add(" -Se encontro");
						listBoxConsola->Items->Add(CantidadErrores);
						listBoxConsola->Items->Add("errores. Memoria corrompida\n\n");
						listBoxConsola->Items->Add("\n  DIR\t\tESPERADO\tLEIDO\n");
						listBoxConsola->Items->Add(CantidadErrores);
	                    listBoxConsola->Items->Add(" ------------------------------------------\n");
	                    for (i = 0; i < CantidadErrores; ++i)
	                        //listBoxConsola->Items->Add(" 0x%2.2X\t\t0x%2.2X\t\t0x%2.2X\n", DireccionErrores[i], ImagenMem_Archivo [DireccionErrores[i]], ImagenMem_Lectura [DireccionErrores[i]] );
						{
							listBoxConsola->Items->Add( DireccionErrores[i] + "\t");
							listBoxConsola->Items->Add( ImagenMem_Archivo[DireccionErrores[i]] + "\t");
							listBoxConsola->Items->Add( ImagenMem_Lectura[DireccionErrores[i]]);
						}
						listBoxConsola->Refresh();
	                    Resultado = ERRACC_DATAV;
	                }
	            }
	            else
	            {
	                listBoxConsola->Items->Add(" -Error en la lectura de memoria.");
					listBoxConsola->Refresh();
	                //Resultado = ERRACC_COM;
	            }
	    }
	    else
	    {
	        listBoxConsola->Items->Add(" -Error al abrir archivo");
			listBoxConsola->Refresh();
	        Resultado = ERRACC_FILE;
	    }
	
	    CrearLog(ACC_VERIF, NombreArchivoLocal, Resultado, DireccionErrores, CantidadErrores);
	}

	void EjecutarPrograma ()
	{
	    listBoxConsola->Items->Add(" EJECUCION DE PROGRAMA");
		label2->Text = "Ejecutando...";
		label2->Refresh();
		ResetMicro(0);                          // Reset para que el micro comience ejecucion de codigo.
	    listBoxConsola->Items->Add(" -Programa ejecutandose");//\t <ENTER> : Detener\n");
		
//	    ResetMicro(0);                          // Reset para que el micro comience ejecucion de codigo.
		
		listBoxConsola->Refresh();
	    /*while (!getch());
	    printf (" -Programa detenido\n");*/
	
	    //ResetMicro(1);                                                      // Reset para entrar al BSL
	
	    //return ERR_OK;
	}

#pragma endregion
};
}

