import { Component, OnInit, ViewChild } from '@angular/core';
import { EvaluacionesService } from '../servicios/evaluaciones.service';
import { ActivatedRoute, Router } from '@angular/router';
import { AuthService } from 'src/app/auth/auth.service';
import { PopupInformativoModel, PopupInformativoComponent } from 'src/app/modales/popup-informativo/popup-informativo.component';
import { MatDialog, PageEvent, MatTableDataSource, MatPaginator, MatSort, MatSortable } from '@angular/material';
import { Observable } from 'rxjs';
import { EvaluacionesListado } from '../comunes/evaluacion';
import { ConfirmDialogModel, DialogoConfirmacionComponent } from 'src/app/modales/dialogo-confirmacion/dialogo-confirmacion.component';

export interface Estado {
  value: string;
  viewValue: string;
}

@Component({
  selector: 'app-listado',
  templateUrl: './listado.component.html',
  styleUrls: ['./listado.component.css']
})
export class ListadoComponent implements OnInit {

  // la agregue por el error del compilador
  public tituloTipo;

  public spinner = true; // arranca con el spinner hasta que obtiene los datos del servidor
  private evaluaciones$: Observable<EvaluacionesListado[]>;
  public todasLasEvaluaciones: EvaluacionesListado[]; // para almacenar los observables como array y usar el metodo slice en la paginacion
  public evaluacionesFiltradas: EvaluacionesListado[] = [];
  selectedId: number;
  estados: Estado[] = [
    {value: 'SI', viewValue: 'Activas'},
    {value: 'NO', viewValue: 'Archivadas'},
    {value: 'CE', viewValue: 'Cerradas'},
    {value: 'ED', viewValue: 'Editables'},
    {value: 'TODAS', viewValue: 'Todas'},
  ];
  public selectedValue: string;
  public idusuario;
  public idsector;
  valorFiltro = '';

  // MatPaginator Inputs
  pageSize = 25;
  pageSizeOptions: number[] = [5, 10, 25, 50, 100];
  // MatPaginator Output
  pageEvent: PageEvent;
  pageIndex = 0;

  displayedColumns: string[] = ['id', 'titulo', 'sector', 'fecha', 'activa', 'menu'];
  dataSource;

  @ViewChild(MatPaginator, {static: true}) paginator: MatPaginator;
  // @ViewChild(MatSort, {static: true}) sort: MatSort;

  constructor(
    private evaluacionesService: EvaluacionesService,
    private rutaActiva: ActivatedRoute,
    private authService: AuthService,
    private router: Router,
    public dialog: MatDialog
  ) { }

  ngOnInit() {
    this.idusuario = this.authService.currentUserValue.id;
    this.idsector = this.authService.currentUserValue.idarea;
    this.selectedValue = 'ED';
    this.evaluacionesService.getListadoEvaluaciones(this.idusuario, this.idsector)
    .subscribe(
      (res) => {
        // console.log(res);
        // tslint:disable: no-string-literal
        if (res['status'] === 200 || res['status'] === 204) {

          this.evaluaciones$ = res['body'];
          // tslint:disable: max-line-length
          this.todasLasEvaluaciones = (this.evaluaciones$ as unknown) as EvaluacionesListado[]; // pasaje para poder utilizar el metodo slice
          this.evaluacionesFiltradas = this.todasLasEvaluaciones.filter( evaluacion => evaluacion.activa === 'CE' || evaluacion.activa === 'SI');
          // this.evaluacionesFiltradas = this.todasLasEvaluaciones;
          // tslint:ensable: max-line-length
          // tslint:enable: no-string-literal
          this.spinner = false;
        } else {
          /*const message = `Lo sentimos ha ocurrido un error en el servidor`;
          const dialogData = new PopupInformativoModel('Error', message, 'danger');
          const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});*/
          this.spinner = false;
        }
      });
  }

  // función para setear el tamaño de la paginación
  setPageSizeOptions(setPageSizeOptionsInput: string) {
    this.pageSizeOptions = setPageSizeOptionsInput.split(',').map(str => +str);
  }

  public handlePage(e: PageEvent): PageEvent {
    this.pageSize = e.pageSize;
    this.pageIndex = e.pageIndex;
    return e;
  }

  refrescarEvaluaciones() {
    this.selectedValue = 'ED';
    this.evaluacionesService.getListadoEvaluaciones(this.idusuario, this.idsector)
    .subscribe(
      (res) => {
        // console.log(res);
        // tslint:disable: no-string-literal
        if (res['status'] === 200 || res['status'] === 204) {
          if (res['status'] === 200) {
            this.evaluaciones$ = res['body'];
            // tslint:disable: max-line-length
            this.todasLasEvaluaciones = (this.evaluaciones$ as unknown) as EvaluacionesListado[]; // pasaje para poder utilizar el metodo slice
            this.evaluacionesFiltradas = this.todasLasEvaluaciones.filter( evaluacion => evaluacion.activa === 'CE' || evaluacion.activa === 'SI');
            // this.evaluacionesFiltradas = this.todasLasEvaluaciones;
            // tslint:ensable: max-line-length
          } else {
            const message = `Aún no hay evaluaciones hechas`;
            const dialogData = new PopupInformativoModel('Aviso', message, 'warning');
            const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});
            this.spinner = false;
          }
          // tslint:enable: no-string-literal
          this.spinner = false;
        } else {
          /*const message = `Lo sentimos ha ocurrido un error en el servidor`;
          const dialogData = new PopupInformativoModel('Error', message, 'danger');
          const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});*/
          this.spinner = false;
        }
      }
    );
  }

  applyFilter(filterValue: string) {
    // this.todosLosEmpleados.filter = filterValue.trim().toLowerCase();
    const filtro = filterValue.trim().toUpperCase();

    let auxFiltroActivos;
    switch (this.selectedValue) {
      case 'SI':
        auxFiltroActivos = this.todasLasEvaluaciones.filter( evaluacion => evaluacion.activa === 'SI');
        break;
      case 'NO':
        auxFiltroActivos = this.todasLasEvaluaciones.filter( evaluacion => evaluacion.activa === 'NO');
        break;
      case 'CE':
        auxFiltroActivos = this.todasLasEvaluaciones.filter( evaluacion => evaluacion.activa === 'CE');
        break;
      case 'ED':
        auxFiltroActivos = this.todasLasEvaluaciones.filter( evaluacion => evaluacion.activa === 'CE' || evaluacion.activa === 'SI');
        break;
      default:
        auxFiltroActivos = this.todasLasEvaluaciones;
        break;
    }

    // tslint:disable-next-line: no-string-literal
    this.evaluacionesFiltradas = auxFiltroActivos.filter( evaluacion => evaluacion.titulo.includes(filtro)
                                                || evaluacion.sector.includes(filtro)
                                                || evaluacion.id.toString().includes(filtro)
                                                || evaluacion.fecha.includes(filtro) );
  }

  onChangeOption() {
    // TODO: amostrar el desplegable segun permisos y acomodar la presentacion
    this.valorFiltro = '';
    let auxFiltroActivos;
    switch (this.selectedValue) {
      case 'SI':
        auxFiltroActivos = this.todasLasEvaluaciones.filter( evaluacion => evaluacion.activa === 'SI');
        break;
      case 'NO':
        auxFiltroActivos = this.todasLasEvaluaciones.filter( evaluacion => evaluacion.activa === 'NO');
        break;
      case 'CE':
        auxFiltroActivos = this.todasLasEvaluaciones.filter( evaluacion => evaluacion.activa === 'CE');
        break;
      case 'ED':
        auxFiltroActivos = this.todasLasEvaluaciones.filter( evaluacion => evaluacion.activa === 'CE' || evaluacion.activa === 'SI');
        break;
      default:
        auxFiltroActivos = this.todasLasEvaluaciones;
        break;
    }
    this.evaluacionesFiltradas = auxFiltroActivos;
  }

  crearEvaluacion() {
    this.router.navigate(['evaluaciones/crear-evaluacion']);
  }

  permisosMenu() {
    if (this.idsector === 1 || this.idsector === 13 || this.idsector === 15 || this.idusuario === 149 || this.idusuario === 304) {
      return true;
    } else {
      return false;
    }
  }

  archivar(evaluacion: EvaluacionesListado) {
    const message = `Está seguro de que quiere hacerlo?`;
    const dialogData = new ConfirmDialogModel('Confirmar', message);
    const dialogRef = this.dialog.open(DialogoConfirmacionComponent, {data: dialogData});

    dialogRef.afterClosed().subscribe(result => {
      if (result === 'true') {

        const data = {
          idevaluacion: evaluacion.id,
          activa: evaluacion.activa
        };

        this.evaluacionesService.putArchivar(JSON.stringify(data)).subscribe(
          (res) => {
            // tslint:disable-next-line: no-string-literal
            if (res['status'] === 200) {
              if (evaluacion.activa === 'SI') {
                evaluacion.activa = 'NO';
              } else {
                evaluacion.activa = 'SI';
              }
            }
          }
        );
      }
    });
  }

  abrirCerrar(evaluacion: EvaluacionesListado) {
    const message = `Está seguro de que quiere hacerlo?`;
    const dialogData = new ConfirmDialogModel('Confirmar', message);
    const dialogRef = this.dialog.open(DialogoConfirmacionComponent, {data: dialogData});

    dialogRef.afterClosed().subscribe(result => {
      if (result === 'true') {

        const data = {
          idevaluacion: evaluacion.id,
          activa: evaluacion.activa
        };

        this.evaluacionesService.putAbrirCerrar(JSON.stringify(data)).subscribe(
          (res) => {
            // tslint:disable-next-line: no-string-literal
            if (res['status'] === 200) {
              if (evaluacion.activa === 'SI') {
                evaluacion.activa = 'CE';
              } else {
                evaluacion.activa = 'SI';
              }
            }
          }
        );
      }
    });
  }

  borrar(evaluacion: EvaluacionesListado) {
    const message = `Está seguro de que quiere hacerlo?`;
    const dialogData = new ConfirmDialogModel('Confirmar', message);
    const dialogRef = this.dialog.open(DialogoConfirmacionComponent, {data: dialogData});

    dialogRef.afterClosed().subscribe(result => {
      if (result === 'true') {

        this.evaluacionesService.deleteEvaluacion(evaluacion.id).subscribe(
          (res) => {
            // tslint:disable-next-line: no-string-literal
            if (res['status'] === 200) {
              this.refrescarEvaluaciones();
            }
          }
        );
      }
    });
  }

  goToEvaluacion(evaluacion: EvaluacionesListado) {
    this.router.navigate(['evaluaciones/' + evaluacion.id + '/' + evaluacion.titulo + '/ranking-y-edicion']);
  }

  get puedeCrear() {
    // tslint:disable: max-line-length
    if (this.idsector === 1 || this.idsector === 15 || this.idusuario === 149 || this.idusuario === 156 ||
      this.idusuario === 284 || this.idusuario === 304) {
      return true;
    } else {
      return false;
    }
    // tslint:enable: max-line-length
  }

}
