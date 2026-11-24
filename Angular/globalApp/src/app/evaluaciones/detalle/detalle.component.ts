import { Component, OnInit, EventEmitter, Output, Input } from '@angular/core';
import { ActivatedRoute, Router } from '@angular/router';
import { AuthService } from 'src/app/auth/auth.service';
import { UtilesEvaluacion, EvaluacionesListado } from '../comunes/evaluacion';
import { EvaluacionesService } from '../servicios/evaluaciones.service';
import { MatDialog } from '@angular/material';
import { PopupInformativoModel, PopupInformativoComponent } from 'src/app/modales/popup-informativo/popup-informativo.component';

@Component({
  selector: 'app-detalle',
  templateUrl: './detalle.component.html',
  styleUrls: ['./detalle.component.css']
})
export class DetalleComponent implements OnInit {
  @Output() guardado = new EventEmitter();
  private idevaluacion: any;
  @Input() public idusuario;
  public spinner = true; // arranca con el spinner hasta que obtiene los datos del servidor
  @Input() public evaluacion: EvaluacionesListado;
  @Input() public evaluado;
  fechaDeLaEvaluacion: Date;
  fechaDeReferencia: Date;
  formatoViejo: boolean;
  public setBackground = UtilesEvaluacion.cambiarfondo;
  public modelos;
  public competencias;
  private empleados;
  public devolucion = {
    id: 0,
    positivo: '',
    negativo: ''
  };
  public empleadoSel;
  public promedioGeneral;
  private puedoDesactivar = true;
  private editar = false;

  constructor(
    private rutaActiva: ActivatedRoute,
    private router: Router,
    private evaluacionesService: EvaluacionesService,
    private authService: AuthService,
    private dialog: MatDialog
  ) {
    this.idevaluacion = this.rutaActiva.snapshot.parent.params.idevaluacion;
    // this.idusuario = this.authService.currentUserValue.id;
  }

  ngOnInit() {
    this.spinner = true;
    this.empleadoSel = this.evaluado;
    this.empleadoSel.id = this.evaluado.evaluadoid;
    this.empleadoSel.nombre = this.evaluado.evaluado;
    this.fechaDeLaEvaluacion = new Date(this.evaluacion.fecha);
    this.fechaDeReferencia = new Date(2019, 4, 10); // (year, month, day)
    if (this.fechaDeLaEvaluacion < this.fechaDeReferencia) {
      this.formatoViejo = true;
    } else {
      this.formatoViejo = false;
    }
    this.obtenerDetalle();
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
        if (competencia.competencia === modelo.competencia && (modelo.ignorar !== 'V')) {
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
      if (modelo.ignorar === 'F') {
        cantidad++;
        total += modelo.puntaje;
      }
    }
    total /= cantidad;
    return total;
  }

  volver() {
    this.guardado.emit(this.idevaluacion);
    this.router.navigate(['/evaluaciones/' + this.idevaluacion + '/' + this.evaluacion.titulo + '/ranking-y-edicion']);
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
