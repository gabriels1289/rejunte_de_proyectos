import { Component, OnInit, Output, EventEmitter, Input } from '@angular/core';
import { EvaluacionesService } from '../servicios/evaluaciones.service';
import { ActivatedRoute, Router } from '@angular/router';
import { AuthService } from 'src/app/auth/auth.service';
import { MatDialog } from '@angular/material';
import { PopupInformativoModel, PopupInformativoComponent } from 'src/app/modales/popup-informativo/popup-informativo.component';
import { UtilesEvaluacion, EvaluacionesListado } from '../comunes/evaluacion';
import { Observable, of } from 'rxjs';
import { ConfirmDialogModel, DialogoConfirmacionComponent } from 'src/app/modales/dialogo-confirmacion/dialogo-confirmacion.component';

@Component({
  selector: 'app-evaluar',
  templateUrl: './evaluar.component.html',
  styleUrls: ['./evaluar.component.css']
})
export class EvaluarComponent implements OnInit {
  @Output() guardado = new EventEmitter();
  private idevaluacion: any;
  private idusuario;
  public spinner = true; // arranca con el spinner hasta que obtiene los datos del servidor
  @Input() public evaluacion: EvaluacionesListado;
  @Input() public evaluado;
  fechaDeLaEvaluacion: Date;
  fechaDeReferencia: Date;
  formatoViejo: boolean;
  public setBackground = UtilesEvaluacion.cambiarfondo;
  public modelos;
  public competencias;
  public empleados;
  public devolucion = {
    id: 0,
    positivo: '',
    negativo: ''
  };
  public empleadoSel;
  public promedioGeneral;
  private puedoDesactivar = true;
  public editar = false;

  constructor(
    private rutaActiva: ActivatedRoute,
    private router: Router,
    private evaluacionesService: EvaluacionesService,
    private authService: AuthService,
    private dialog: MatDialog
  ) {
    this.idevaluacion = this.rutaActiva.snapshot.parent.params.idevaluacion;
    this.idusuario = this.authService.currentUserValue.id;
  }

  ngOnInit() {
    this.spinner = true;
    if (this.evaluacion === undefined) {
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
            this.obtenerModelo();
            // tslint:enable: no-string-literal
          } else {
            /*const message = `Lo sentimos ha ocurrido un error en el servidor`;
            const dialogData = new PopupInformativoModel('Error', message, 'danger');
            const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});*/
            this.spinner = false;
          }
        });
    } else {
      this.editar = true;
      this.empleadoSel = this.evaluado;
      this.empleadoSel.id = this.evaluado.evaluadoid;
      this.fechaDeLaEvaluacion = new Date(this.evaluacion.fecha);
      this.fechaDeReferencia = new Date(2019, 4, 10); // (year, month, day)
      if (this.fechaDeLaEvaluacion < this.fechaDeReferencia) {
        this.formatoViejo = true;
      } else {
        this.formatoViejo = false;
      }
      this.obtenerDetalle();
    }
  }

  obtenerModelo() {
    this.evaluacionesService.getParaEvaluar(this.idevaluacion, this.evaluacion.idsector, this.idusuario, this.evaluacion.modelo)
    .subscribe(
      (res) => {
        // console.log(res);
        // tslint:disable: no-string-literal
        if (res['status'] === 200 || res['status'] === 204) {
          this.modelos = res['body'][0][0];
          this.competencias = res['body'][0][1];

          for (const competencia of this.competencias) {
            competencia.porcentaje = 0;
          }

          for (const modelo of this.modelos) {
            modelo.puntaje = -1;
            // modelo.ignorar = 'F';
            modelo.ignorar = false;
          }

          this.empleados = res['body'][1];
          this.empleadoSel = this.empleados[0];

          this.spinner = false;
          // tslint:enable: no-string-literal
        } else {
          /*const message = `Lo sentimos ha ocurrido un error en el servidor`;
          const dialogData = new PopupInformativoModel('Error', message, 'danger');
          const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});*/
          this.spinner = false;
        }
      });
  }

  obtenerDetalle() {
    this.evaluacionesService.getDetalleEvaluacion(this.evaluacion.id, this.idusuario, this.evaluado.evaluadoid)
    .subscribe(
      (res) => {
        // console.log(res);
        // tslint:disable: no-string-literal
        if (res['status'] === 200 || res['status'] === 204) {
          this.modelos = res['body'][0];
          this.competencias = res['body'][1];
          this.empleadoSel.foto = res['body'][2][0].foto;
          this.devolucion = res['body'][3][0];

          for (const competencia of this.competencias) {
            competencia.porcentaje = 0;
          }

          for (const modelo of this.modelos) {
            if (modelo.ignorar === 'F') {
              modelo.ignorar = false;
            } else {
              modelo.ignorar = true;
            }
          }

          this.calcularPorcentajes();
          this.puedoDesactivar = true;

          this.spinner = false;
          // tslint:enable: no-string-literal
        } else {
          /*const message = `Lo sentimos ha ocurrido un error en el servidor`;
          const dialogData = new PopupInformativoModel('Error', message, 'danger');
          const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});*/
          this.spinner = false;
        }
      });
  }

  onChangeSelect() {
    for (const modelo of this.modelos) {
      modelo.puntaje = -1;
      modelo.ignorar = false;
    }
    this.calcularPorcentajes();
    this.devolucion.positivo = '';
    this.devolucion.negativo = '';
    this.puedoDesactivar = true;
  }

  cambiosDevolucion() {
    if (this.puedoDesactivar === true) {
      this.puedoDesactivar = false;
    }
  }

  calcularPorcentajes() {
    if (this.puedoDesactivar === true) {
      this.puedoDesactivar = false;
    }
    for (const competencia of this.competencias) {
      competencia.porcentaje = 0;
    }

    for (const competencia of this.competencias) {
      let cantidad = 0;
      for (const modelo of this.modelos) {
        if (competencia.competencia === modelo.competencia && (modelo.ignorar !== true)) {
          cantidad++;
          competencia.porcentaje += modelo.puntaje;
        }
      }
      competencia.porcentaje /= cantidad;
    }
    this.promedioGeneral = this.calcularPromedio();
  }

  calcularPromedio(): number {
    let total = 0;
    let cantidad = 0;
    for (const modelo of this.modelos) {
      if (modelo.ignorar === false) {
        cantidad++;
        total += modelo.puntaje;
      }
    }
    total /= cantidad;
    return total;
  }

  test() {
    console.log('empleadoSel: %o', this.empleadoSel);
    console.log('modelos: %o', this.modelos);
    console.log('puedoDesactivar: %o', this.puedoDesactivar);
  }

  guardar() {
    let puntajeNegativo = false;
    for (const modelo of this.modelos) {
      // acomodo el valor de los ignorar
      if (modelo.ignorar === true) {
        modelo.ignorar = 'V';
      } else {
        modelo.ignorar = 'F';
      }
      // compruebo que los valores sean correctos
      if (modelo.ignorar === 'V') {
        modelo.puntaje = -1;
      }
      if (modelo.puntaje == null) {
        modelo.puntaje = -1;
        modelo.ignorar = 'F';
      }
      if (modelo.puntaje === -1 && modelo.ignorar === 'F') {
        puntajeNegativo = true;
      }
    }
    if (isNaN(this.promedioGeneral)) {
      puntajeNegativo = true;
    }
    if (puntajeNegativo === true) {
      const message = `Lo sentimos, hay un campo de evaluación con puntaje fuera de rango. Por favor verifique e intente de nuevo`;
      const dialogData = new PopupInformativoModel('Error', message, 'danger');
      const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});
    } else {
      const message = `Está seguro de que desea guardar los cambios?`;
      const dialogData = new ConfirmDialogModel('Confirmar', message);
      const dialogRef = this.dialog.open(DialogoConfirmacionComponent, {data: dialogData});

      dialogRef.afterClosed().subscribe(result => {
        if (result === 'true') {
          this.spinner = true;

          const data = {
            idevaluacion: this.evaluacion.id,
            idsupervisor: this.idusuario,
            idevaluado: this.empleadoSel.id,
            modelo: this.modelos,
            positivo: this.devolucion.positivo,
            negativo: this.devolucion.negativo,
            iddevolucion: this.devolucion.id,
            editar: this.editar
          };

          const formData = new FormData();
          formData.append('data', JSON.stringify(data));

          this.evaluacionesService.postEvaluacionEmpleado(formData).subscribe(
            (res) => {
              // tslint:disable: no-string-literal
              // console.log('respuesta: %o', res);
              this.spinner = false;
              if (res['status'] === 200) {
                this.puedoDesactivar = true;
                this.guardado.emit(this.idevaluacion);
                this.router.navigate(['/evaluaciones/' + this.idevaluacion + '/' + this.evaluacion.titulo + '/ranking-y-edicion']);
              } else {
                /*const message2 = `Lo sentimos ha ocurrido un error en el servidor`;
                const dialogData2 = new PopupInformativoModel('Error', message2, 'danger');
                const dialogRef2 = this.dialog.open(PopupInformativoComponent, {data: dialogData2});*/
              }
              // tslint:disable: no-string-literal
            }
          );
        }
      });
    }
  }

  cancelarEdicion() {
    this.guardado.emit(this.idevaluacion);
    this.router.navigate(['/evaluaciones/' + this.idevaluacion + '/' + this.evaluacion.titulo + '/ranking-y-edicion']);
  }

  canDeactivate(): Observable<boolean> | boolean {
    // console.log('misArchivos.length: %o', this.misArchivos.length);
    // tslint:disable-next-line: max-line-length
    if (this.puedoDesactivar === false) {
      /* const message = `Está seguro de que quiere descartar los cambios?`;
      const dialogData = new ConfirmDialogModel('Confirmar', message);
      const dialogRef = this.dialog.open(DialogoConfirmacionComponent, dialogConfig);
      dialogRef.afterClosed().subscribe(result => {
        return of(result);
      }); */
      // TODO: mejorar y usar un modal
      const confirmation = window.confirm('quiere descartar los cambios?' || 'Is it OK?');
      return of(confirmation);
    }
    return true;
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

  // TODO: funciones calculos de promedio y porcentajes -> ver si puedo pasarlas como globales
  // TODO: mejorar la validacion visual

}
