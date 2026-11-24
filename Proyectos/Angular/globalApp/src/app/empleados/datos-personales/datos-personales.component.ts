import { Component, OnInit } from '@angular/core';
import { ActivatedRoute, Router, ResolveStart } from '@angular/router';
import { EmpleadosService } from '../servicios/empleados.service';
import { AuthService } from 'src/app/auth/auth.service';
import { PopupInformativoModel, PopupInformativoComponent } from 'src/app/modales/popup-informativo/popup-informativo.component';
import { MatDialog } from '@angular/material';
import { ConfirmDialogModel, DialogoConfirmacionComponent } from 'src/app/modales/dialogo-confirmacion/dialogo-confirmacion.component';
import { Empleado } from '../comunes/empleado';
import { Rol } from 'src/app/_models/roles';

import pdfMake from 'pdfmake/build/pdfmake';
import pdfFonts from 'pdfmake/build/vfs_fonts';
import { formatDate } from '@angular/common';
import { HistorialSeguimiento } from 'src/app/seguimientos/comunes/seguimiento';
import { UtilesMensajeria } from 'src/app/mensajeria/comunes/correo';
pdfMake.vfs = pdfFonts.pdfMake.vfs;

@Component({
  selector: 'app-datos-personales',
  templateUrl: './datos-personales.component.html',
  styleUrls: ['./datos-personales.component.css']
})
export class DatosPersonalesComponent implements OnInit {

  public spinner = true;
  private idempleado;
  public empleado$: Empleado;
  public editar = false;
  public idacceso;
  private rol;
  private docDefinition; // para generar el pdf
  private seguimientos$;
  private seguimientos;
  private accionReporte;

  constructor(
    private rutaActiva: ActivatedRoute,
    private router: Router,
    private empleadosService: EmpleadosService,
    private authService: AuthService,
    private dialog: MatDialog
  ) {
    //#region comentarios
    /* const parentActivatedRoute = this.rutaActiva.parent.params.subscribe(
    (params) => {
      console.log('suscribe: %o', params.idempleado);
    }); */
    //#endregion
    this.idempleado = this.rutaActiva.snapshot.parent.params.idempleado;
    this.idacceso = this.authService.currentUserValue.idacceso;
    this.rol = this.authService.currentUserValue.rol;
  }

  ngOnInit() {
    this.obtenerDetalles();
  }

  obtenerDetalles() {
    this.empleadosService.getDetalleEmpleado(this.idempleado).subscribe(
      (res) => {
        // console.log('Res: %o', res);
        // tslint:disable: no-string-literal
        if (res['status'] === 200 || res['status'] === 204) {
          this.empleado$ = res['body'];
          this.spinner = false;
        } else {
          this.spinner = false;
         /* const message = `Lo sentimos ha ocurrido un error en el servidor`;
          const dialogData = new PopupInformativoModel('Error', message, 'danger');
          const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData}); */
        }
        // tslint:enable: no-string-literal
      }
    );
  }

  resetearPassword() {
    const message = `Está seguro de que quiere hacerlo?`;
    const dialogData = new ConfirmDialogModel('Confirmar', message);
    const dialogRef = this.dialog.open(DialogoConfirmacionComponent, {data: dialogData});

    dialogRef.afterClosed().subscribe(result => {
      if (result === 'true') {
        const data = {
          idempleado: this.idempleado,
          numerodocumento: this.empleado$.numerodocumento
        };

        this.empleadosService.resetearPassword(JSON.stringify(data)).subscribe(
          (res) => {
            // console.log('Res: %o', res);
            // tslint:disable-next-line: no-string-literal
            if (res['status'] === 200) {
              const message2 = `Cambio efectuado con éxito`;
              const dialogData2 = new PopupInformativoModel('Éxito', message2, 'success');
              const dialogRef2 = this.dialog.open(PopupInformativoComponent, {data: dialogData2});
            } else {
             /* const message2 = `Lo sentimos ha ocurrido un error en el servidor`;
              const dialogData2 = new PopupInformativoModel('Error', message2, 'danger');
              const dialogRef2 = this.dialog.open(PopupInformativoComponent, {data: dialogData2});*/
            }
          }
        );
      }
    });
  }

  editarEmpleado() {
    this.editar = true;
  }

  get permisoEdicion() {
    // tslint:disable-next-line: max-line-length
    if (this.rol === Rol.Sistemas || this.rol === Rol.AdministracionI || this.rol === Rol.ControlDeJuegoIII || this.rol === Rol.ControlDeJuegoII) {
      return true;
    } else {
      return false;
    }
  }

  get permisoReporte() {
    // tslint:disable-next-line: max-line-length
    if (this.rol === Rol.Sistemas || this.rol === Rol.AdministracionI || this.rol === Rol.ControlDeJuegoIII || this.rol === Rol.ControlDeJuegoII) {
      return true;
    } else {
      return false;
    }
  }

  obtenerSeguimientos() {
    this.empleadosService.getHistorialDeSeguimientos(this.idempleado)
    .subscribe(
      (res) => {
        // console.log(res);
        // tslint:disable: no-string-literal
        if (res['status'] === 200 || res['status'] === 204) {
          if (res['status'] === 200) {
            this.seguimientos$ = res['body'];
            // tslint:disable-next-line: max-line-length
            this.seguimientos = (this.seguimientos$ as unknown) as HistorialSeguimiento[]; // pasaje para poder utilizar el metodo slice
            for (const seguimiento of this.seguimientos) {
              seguimiento.detalle = UtilesMensajeria.acomodarString(seguimiento.detalle);
            }
          }
          this.generarTabla();
          this.spinner = false;
        } else {
         /* const message = `Lo sentimos ha ocurrido un error en el servidor`;
          const dialogData = new PopupInformativoModel('Error', message, 'danger');
          const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData}); */
          this.spinner = false;
        }
        // tslint:enable: no-string-literal
      });
  }

  obtenerBody() {
    const segs = [];
    const encabezadoTabla = [
      {text: 'Fecha', style: 'header'}, {text: 'Efectuado por:', style: 'header'},
      {text: 'Tipo', style: 'header'}, {text: 'Cal.', style: 'header'},
      {text: 'Detalle', style: 'header'}
    ];
    segs.push(encabezadoTabla);
    if ( this.seguimientos == null) {
      const aux = [
        ' ',
        ' ',
        ' ',
        ' ',
        ' '
      ];
      segs.push(aux);
    } else {
      for (const seguimiento of this.seguimientos) {
        const aux = [
          seguimiento.fecha,
          seguimiento.supervisor,
          seguimiento.observacion,
          seguimiento.calificacion,
          seguimiento.detalle
        ];
        segs.push(aux);
      }
    }
    return segs;
  }

  generarTabla() {
    // tslint:disable: max-line-length
    this.docDefinition = {
      /*footer: function(currentPage, pageCount) { return currentPage.toString() + ' of ' + pageCount; },
			header: function(currentPage, pageCount) {
				// you can apply any logic and return any valid pdfmake element
				return { text: currentPage.toString() + ' of ' + pageCount, alignment: (currentPage % 2) ? 'left' : 'right' };
			},*/
      footer: (currentPage, pageCount) => {
        // you can apply any logic and return any valid pdfmake element
        return { text: currentPage.toString() + ' de ' + pageCount, alignment: 'right', margin: [0, 0, 20, 0] }; // [left, top, right, bottom]
      },
      content: [
        {
          text: this.empleado$.apellido + ', ' + this.empleado$.nombre + '\n\n',
          style: 'titulos'
        },
        {
          // style: 'demoTable',
          /*table: {
						widths: ['*', '*', '*', '*'],
						body: obtenerBodyEmpleado()
					}*/
          text: [
                  {text: 'Datos Personales\n\n', style: 'subtitulos'},
                  {text: 'Fecha de nacimiento: ', bold: true}, {text: formatDate(this.empleado$.fechanacimiento, 'MMM d, y', 'es-AR') + '\t\t', style: 'demoTable'},
                  {text: 'Sexo: ', bold: true}, {text: this.empleado$.sexo + '\n\n', style: 'demoTable'},
                  {text: 'Tipo de documento: ', bold: true}, {text: this.empleado$.tipodocumento + '\t\t', style: 'demoTable'},
                  {text: 'Número de documento: ', bold: true}, {text: this.empleado$.numerodocumento + '\n\n', style: 'demoTable'},
                  {text: 'Localidad: ', bold: true}, {text: this.empleado$.localidad + '\t\t', style: 'demoTable'},
                  {text: 'Código postal: ', bold: true}, {text: this.empleado$.codigopostal + '\n\n', style: 'demoTable'},
                  {text: 'Dirección: ', bold: true}, {text: this.empleado$.direccion + '\t', style: 'demoTable'},
                  {text: 'Mail: ', bold: true}, {text: this.empleado$.mail + '\n\n', style: 'demoTable'},
                  {text: 'Teléfono celular: ', bold: true}, {text: this.empleado$.telefonomovil + '\t\t', style: 'demoTable'},
                  {text: 'Teléfono fijo: ', bold: true}, {text: this.empleado$.telefonofijo + '\n\n', style: 'demoTable'},
                  {text: 'Datos Laborales\n\n', style: 'subtitulos'},
                  {text: 'Activo: ', bold: true}, {text: this.empleado$.activo + '\t', style: 'demoTable'},
                  {text: 'Número de legajo: ', bold: true}, {text: this.empleado$.legajo + '\t', style: 'demoTable'},
                  {text: 'Cuil: ', bold: true}, {text: this.empleado$.cuil + '\n\n', style: 'demoTable'},
                  // {text: 'Fecha incorporacion: ', bold: true},{text: $filter('date')($scope.empleado.fechaincorporacion, 'mediumDate')+'\t', style: 'demoTable'},
                  {text: 'Fecha contrato: ', bold: true}, {text: formatDate(this.empleado$.fechacontrato, 'MMM d, y', 'es-AR') + '\t', style: 'demoTable'},
                  {text: 'Fecha baja: ', bold: true}, {text: formatDate(this.empleado$.fechabaja, 'MMM d, y', 'es-AR') + '\n\n', style: 'demoTable'},
                  {text: 'Empresa: ', bold: true}, {text: this.empleado$.razonsocial + '\t\t', style: 'demoTable'},
                  {text: 'Sector: ', bold: true}, {text: this.empleado$.sector + '\n\n', style: 'demoTable'},
                  {text: 'Básico: ', bold: true}, {text: this.empleado$.basico + '\t\t', style: 'demoTable'},
                  {text: 'Full time: ', bold: true}, {text: this.empleado$.fulltime + '\n\n', style: 'demoTable'},
                  {text: 'Obra social: ', bold: true}, {text: this.empleado$.obrasocial + '\t', style: 'demoTable'},
                  {text: 'ART: ', bold: true}, {text: this.empleado$.art + '\n\n', style: 'demoTable'},
                  {text: 'Datos Preocupacional\n\n', style: 'subtitulos'},
                  {text: 'Fecha análisis: ', bold: true}, {text: formatDate(this.empleado$.fechaanalisis, 'MMM d, y', 'es-AR') + '\t\t', style: 'demoTable'},
                  {text: 'Legajo estudio: ', bold: true}, {text: this.empleado$.legajoestudio + '\t\t', style: 'demoTable'},
                  {text: 'Preocupacional: ', bold: true}, {text: this.empleado$.preocupacional + '\n\n', style: 'demoTable'},
                  {text: 'Detalles del preocupacional: ', bold: true}, {text: this.empleado$.preocupacionaldetalle + '\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n', style: 'demoTable'},
          ]
        },
        {
          text: '\n\nSeguimientos\n\n',
          style: 'subtitulos'
        },
        {
          style: 'demoTable',
          table: {
            widths: ['12%', '20%', '15%', '5%', '*'],
            body: this.obtenerBody()
          }
        }
      ],
      styles: {
        header: {
          bold: true,
          color: '#000',
          fontSize: 11
        },
        demoTable: {
          color: '#666',
          fontSize: 10
        },
        subtitulos: {
          fontSize: 18,
          bold: true,
          alignment: 'center'
        },
        titulos: {
          fontSize: 22,
          bold: true,
          alignment: 'center'
        }
      }
    };

    const fecha = new Date();
    const fechaActual = formatDate(fecha, 'yyyy-MM-dd', 'es-AR');
    switch (this.accionReporte) {
      case 'descargar':
        pdfMake.createPdf(this.docDefinition).download('reporteDe - ' + this.empleado$.apellido + ', ' + this.empleado$.nombre + ' - ' + fechaActual + '.pdf');
        break;
      /* case 'abrir':
        pdfMake.createPdf(this.docDefinition).open();
        break;
      case 'imprimir':
        pdfMake.createPdf(this.docDefinition).print();
        break; */
      default:
        break;
    }
    // tslint:enable: max-line-length
  }

  generarReporte(accion) {
    this.accionReporte = accion;
    this.obtenerSeguimientos();
    // this.generarTabla();
  }

  onGuardado(id) {
    this.idempleado = id;
    this.editar = false;
    this.obtenerDetalles();
  }

  // TODO: implementar alguna estrategia para el canDeactive

}
