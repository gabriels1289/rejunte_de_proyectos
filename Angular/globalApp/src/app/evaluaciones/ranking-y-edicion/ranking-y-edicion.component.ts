import { Component, OnInit } from '@angular/core';
import { ActivatedRoute, Router } from '@angular/router';
import { EvaluacionesService } from '../servicios/evaluaciones.service';
import { AuthService } from 'src/app/auth/auth.service';
import { MatDialog } from '@angular/material';
import { EvaluacionesListado, UtilesEvaluacion } from '../comunes/evaluacion';
import { PopupInformativoModel, PopupInformativoComponent } from 'src/app/modales/popup-informativo/popup-informativo.component';

@Component({
  selector: 'app-ranking-y-edicion',
  templateUrl: './ranking-y-edicion.component.html',
  styleUrls: ['./ranking-y-edicion.component.css']
})
export class RankingYEdicionComponent implements OnInit {
  idevaluacion: any;
  idusuario: number;
  public spinner = true; // arranca con el spinner hasta que obtiene los datos del servidor
  public evaluacion: EvaluacionesListado;
  fechaDeLaEvaluacion: Date;
  fechaDeReferencia: Date;
  formatoViejo: boolean;
  public setBackground = UtilesEvaluacion.cambiarfondo;
  idsupervisor: number;
  columnas: any;
  filas: any;
  comparaciones: any;
  idacceso: number;
  bigBrother = false;
  supervisores;
  supervisorSel;
  editar = false;
  filaEvaluado: any;
  detalle = false;

  constructor(
    private rutaActiva: ActivatedRoute,
    private router: Router,
    private evaluacionesService: EvaluacionesService,
    private authService: AuthService,
    private dialog: MatDialog
  ) {
    this.idevaluacion = this.rutaActiva.snapshot.parent.params.idevaluacion;
    this.idusuario = this.authService.currentUserValue.id;
    this.idacceso = this.authService.currentUserValue.idacceso;
  }

  ngOnInit() {
    this.spinner = true;
    this.obtenerResumenEvaluacion();
    if (this.idacceso === 1 || this.idacceso === 10) {
      this.listadoBigBrother();
    }
  }

  obtenerResumenEvaluacion() {
    this.evaluacionesService.getResumenEvaluacion(this.idevaluacion)
    .subscribe(
      (res) => {
        // console.log(res);
        // tslint:disable: no-string-literal
        if (res['status'] === 200) {
          this.evaluacion = res['body'];
          // compruebo las fechas de la evaluacion con la del cambio de formato para ver cual debo usar
          this.fechaDeLaEvaluacion = new Date(this.evaluacion.fecha);
          this.fechaDeReferencia = new Date(2019, 4, 10); // (year, month, day)

          if (this.fechaDeLaEvaluacion < this.fechaDeReferencia) {
            this.formatoViejo = true;
          } else {
            this.formatoViejo = false;
          }
          this.obtenerRanking();
          // tslint:enable: no-string-literal
        } else {
          /*const message = `Lo sentimos ha ocurrido un error en el servidor`;
          const dialogData = new PopupInformativoModel('Error', message, 'danger');
          const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});*/
          this.spinner = false;
        }
      });
  }

  obtenerRanking() {
    if (this.bigBrother === true) {
      this.idsupervisor = this.supervisorSel.id;
    } else {
      this.idsupervisor = this.idusuario;
    }
    this.evaluacionesService.getRankingEvaluacion(this.idevaluacion, this.idsupervisor)
    .subscribe(
      (res) => {
        // console.log(res);
        // tslint:disable: no-string-literal
        if (res['status'] === 200) {
          this.columnas = res['body'][0];
          this.filas = res['body'][1];
          this.comparaciones = res['body'][2];
          this.spinner = false;
        } else if (res['status'] === 204) {
          this.columnas = null;
          this.filas = null;
          this.comparaciones = null;
          const message = `No hay evaluaciones hechas`;
          const dialogData = new PopupInformativoModel('Aviso', message, 'primary');
          const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});
          this.spinner = false;
        } else {
          /*const message = `Lo sentimos ha ocurrido un error en el servidor`;
          const dialogData = new PopupInformativoModel('Error', message, 'danger');
          const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});*/
          this.spinner = false;
        }
        // tslint:enable: no-string-literal
      });
  }

  listadoBigBrother() {
    this.evaluacionesService.getListadoBigBrother(this.idevaluacion, this.idusuario)
    .subscribe(
      (res) => {
        // console.log(res);
        // tslint:disable: no-string-literal
        if (res['status'] === 200) {
          this.supervisores = res['body'];
          this.supervisorSel = this.supervisores[0];
        } else {
          /*const message = `Lo sentimos ha ocurrido un error en el servidor`;
          const dialogData = new PopupInformativoModel('Error', message, 'danger');
          const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});*/
          this.spinner = false;
        }
        // tslint:enable: no-string-literal
      });
  }

  callBigBrother() {
    this.obtenerRanking();
  }

  verDetalle(fila) {
    this.filaEvaluado = fila;
    this.detalle = true;
  }

  edicion(fila) {
    this.filaEvaluado = fila;
    this.editar = true;
  }

  onGuardado(id) {
    // this.idevaluacion = id;
    this.editar = false;
    this.detalle = false;
    this.obtenerResumenEvaluacion();
  }

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
}
