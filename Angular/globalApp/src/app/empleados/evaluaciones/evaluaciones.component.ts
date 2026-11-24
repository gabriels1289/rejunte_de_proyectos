import { Component, OnInit } from '@angular/core';
import { ActivatedRoute, Router } from '@angular/router';
import { EmpleadosService } from '../servicios/empleados.service';
import { AuthService } from 'src/app/auth/auth.service';
import { MatDialog } from '@angular/material';
import { PopupInformativoModel, PopupInformativoComponent } from 'src/app/modales/popup-informativo/popup-informativo.component';
import { UtilesEvaluacion } from 'src/app/evaluaciones/comunes/evaluacion';

@Component({
  selector: 'app-evaluaciones',
  templateUrl: './evaluaciones.component.html',
  styleUrls: ['./evaluaciones.component.css']
})
export class EvaluacionesComponent implements OnInit {

  // esta propiedad la agrego por error del compilador
  public mostrar = false;

  private idevaluado;
  private origen = 'empleadoevaluacion';
  public evaluaciones;
  public selectedValue;
  public mostrarEval = false;
  public mostrarSelect = false;
  public spinner = false;
  form: any;
  modelos: any;
  competencias: any;
  supervisores: any;
  puntajes: any;
  columnas: any;
  filas: any;
  devoluciones: any;
  devolucion: any;
  promedioGeneral = 0;
  tituloEvaluacion: any;
  fechaDeLaEvaluacion: Date;
  fechaDeReferencia: Date;
  formatoViejo: boolean;
  // cambiarfondo: Function;
  public setBackground = UtilesEvaluacion.cambiarfondo;
  apellido;
  nombre;

  constructor(
    private rutaActiva: ActivatedRoute,
    private router: Router,
    private empleadosService: EmpleadosService,
    private authService: AuthService,
    private dialog: MatDialog
  ) {
    this.idevaluado = this.rutaActiva.snapshot.parent.params.idempleado;
  }

  ngOnInit() {
    this.nombre = this.rutaActiva.snapshot.parent.params.nombre;
    this.apellido = this.rutaActiva.snapshot.parent.params.apellido;
    this.obtenerListado();
  }

  obtenerListado() {
    this.empleadosService.getEvaluacionesDeEmpleado(this.idevaluado, this.origen).subscribe(
      (res) => {
        // console.log('Res: %o', res);
        // tslint:disable: no-string-literal
        if (res['status'] === 200 || res['status'] === 204) {
          if (res['status'] === 200) {
            this.evaluaciones = res['body'];
            this.mostrarSelect = true;
          } else {
            this.mostrarSelect = false;
            // this.mostrarEval = false;
            const message = `Aún no hay evaluaciones para mostrar`;
            const dialogData = new PopupInformativoModel('Aviso', message, 'warning');
            const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});
          }
        } else {
          // this.mostrarEval = false;
          this.mostrarSelect = false;
          /*const message = `Lo sentimos ha ocurrido un error en el servidor`;
          const dialogData = new PopupInformativoModel('Error', message, 'danger');
          const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});*/
        }
        // tslint:enable: no-string-literal
      }
    );
  }

  obtenerLaEvaluacion() {
    this.mostrarEval = false;
    this.mostrarSelect = false;
    this.spinner = true;
    this.empleadosService.getDetalleEvaluacionEmpleado(this.idevaluado, this.selectedValue.evaluacionid).subscribe(
      (res) => {
        // console.log('Res: %o', res);
        // tslint:disable: no-string-literal
        if (res['status'] === 200 || res['status'] === 204) {
          if (res['status'] === 200) {
            this.modelos = res['body'][0][0];
            this.competencias = res['body'][0][1];
            this.supervisores = res['body'][0][2];
            this.puntajes = res['body'][0][3];

            this.columnas = res['body'][1][0];
            this.filas = res['body'][1][1];
            this.devoluciones = res['body'][1][3];

            this.devolucion = res['body'][2];

            for (const puntaje of this.puntajes) {
              puntaje.puntaje = +puntaje.puntaje;
            }

            for (const modelo of this.modelos) {
              modelo.promedio = +modelo.promedio;
            }

            for (const supervisor of this.supervisores) {
              supervisor.promedio = +supervisor.promedio;
            }

            // this.calcularPorcentajes();
            this.competencias = UtilesEvaluacion.calcularPorcentajesVisual(this.competencias, this.modelos);
            // this.promedioGeneral = this.calcularPromedio();
            this.promedioGeneral = UtilesEvaluacion.calcularPromedioVisual(this.modelos);
            this.tituloEvaluacion = this.selectedValue.texto;

            // compruebo las fechas de la evaluacion con la del cambio de formato para ver cual debo usar
            this.fechaDeLaEvaluacion = new Date(this.selectedValue.fecha);
            this.fechaDeReferencia = new Date(2019, 4, 10); // (year, month, day)

            if (this.fechaDeLaEvaluacion < this.fechaDeReferencia) {
              this.formatoViejo = true;
            } else {
              this.formatoViejo = false;
            }

            this.mostrarEval = true;
            this.spinner = false;
            this.mostrarSelect = true;
          } else {
            // TODO: mostrar mensaje que no hay evaluaciones para mostrar
            const message = `Aún no hay evaluaciones para mostrar`;
            const dialogData = new PopupInformativoModel('Aviso', message, 'warning');
            const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});
          }
        } else {
          this.mostrarEval = false;
          this.spinner = false;
          this.mostrarSelect = true;
          /*const message = `Lo sentimos ha ocurrido un error en el servidor`;
          const dialogData = new PopupInformativoModel('Error', message, 'danger');
          const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});*/
        }
        // tslint:enable: no-string-literal
      }
    );
  }

  onChangeOption() {
    this.obtenerLaEvaluacion();
  }

 /*  // TODO: ver si puedo colocar el calculo de puntaje y el promedio como una utilidad en evaluacion.ts
  calcularPorcentajes() {
    for (const competencia of this.competencias) {
      competencia.porcentaje = 0;
    }

    for (const competencia of this.competencias) {
      let cantidad = 0;
      for (const modelo of this.modelos) {
        if (competencia.competencia === modelo.competencia && (modelo.promedio !== -1)) {
          cantidad++;
          competencia.porcentaje += modelo.promedio;
        }
      }
      competencia.porcentaje /= cantidad;
    }
  }

  calcularPromedio(): number {
    let total = 0;
    let cantidad = 0;
    for (const modelo of this.modelos) {
      if (modelo.promedio !== -1) {
        cantidad++;
        total += modelo.promedio;
      }
    }
    total /= cantidad;
    return total;
  } */

  //#region funciones trackBy
  public trackByFunctionIdSup(index, item) {
    if (!item) {
      return null;
    }
    return item.idsup; // or return index
  }

  public trackByFunctionId(index, item) {
    if (!item) {
      return null;
    }
    return item.id; // or return index
  }

  public trackByFunctionIndex(index, item) {
    return index; // or return index
  }

  public trackByFunctionIdPe(index, item) {
    if (!item) {
      return null;
    }
    return item.idpe; // or return index
  }
  //#endregion

  getReporteExcel() {
    const tituloReporte = this.tituloEvaluacion;
    const nombreExcel = (this.apellido + ', ' + this.nombre + ' - ' + this.tituloEvaluacion);
    this.empleadosService.getReporteEvaluacionExcel(this.idevaluado, this.selectedValue.evaluacionid, tituloReporte, nombreExcel).subscribe(
      (response: any) => {
        const dataType = response.type;
        const binaryData = [];
        binaryData.push(response);
        const downloadLink = document.createElement('a');
        downloadLink.href = window.URL.createObjectURL(new Blob(binaryData, {type: dataType}));
        if (nombreExcel) {
          downloadLink.setAttribute('download', (nombreExcel + '.xlsx'));
        }
        document.body.appendChild(downloadLink);
        downloadLink.click();
        downloadLink.parentNode.removeChild(downloadLink);
      }
    );
  }

  // TODO: mejorar el modal colocando uno de angular material y en lugar de imagenes utilizar divs con backgrounds
  // esto es referido a las referencias

}
