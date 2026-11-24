import { Component, OnInit, ViewChild } from '@angular/core';
import { EmpleadosService } from '../servicios/empleados.service';
import { ActivatedRoute, Router } from '@angular/router';
import { AuthService } from 'src/app/auth/auth.service';
import { MatDialog, PageEvent, MatPaginator, MatDialogConfig } from '@angular/material';
import { HistorialSeguimiento } from 'src/app/seguimientos/comunes/seguimiento';
import { Observable } from 'rxjs';
import { PopupInformativoComponent, PopupInformativoModel } from 'src/app/modales/popup-informativo/popup-informativo.component';
import { UtilesMensajeria } from 'src/app/mensajeria/comunes/correo';
import { ConfirmDialogModel, DialogoConfirmacionComponent } from 'src/app/modales/dialogo-confirmacion/dialogo-confirmacion.component';
import { NuevoSeguimientoComponent } from '../modales/nuevo-seguimiento/nuevo-seguimiento.component';

@Component({
  selector: 'app-seguimientos',
  templateUrl: './seguimientos.component.html',
  styleUrls: ['./seguimientos.component.css']
})
export class SeguimientosComponent implements OnInit {

  // la agregue por error del compilador
  public valorFiltro;

  private idacceso;
  private idempleado;
  private idsector;
  private seguimientos$: Observable<HistorialSeguimiento[]>;
  public todosLosSeguimientos: HistorialSeguimiento[];
  public seguimientosFiltrados: HistorialSeguimiento[] = [];
  public permisoSeguimiento;
  public spinner;

  // MatPaginator Inputs
  pageSize = 10;
  pageSizeOptions: number[] = [5, 10, 25, 50, 100];
  // MatPaginator Output
  pageEvent: PageEvent;
  pageIndex = 0;

  displayedColumns: string[] = ['fecha', 'supervisor', 'observacion', 'calificacion', 'detalle', 'borrar'];

  @ViewChild(MatPaginator, {static: true}) paginator: MatPaginator;

  constructor(
    private empleadosService: EmpleadosService,
    private rutaActiva: ActivatedRoute,
    private authService: AuthService,
    private router: Router,
    public dialog: MatDialog
  ) {
  }

  ngOnInit() {
    this.idempleado = this.rutaActiva.snapshot.parent.params.idempleado;
    this.idsector = this.rutaActiva.snapshot.parent.params.idsector;
    this.idacceso = this.authService.currentUserValue.idacceso;
    this.puedeHacerSeguimiento();
    this.obtenerSeguimientos();
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

  applyFilter(filterValue: string) {
    const filtro = filterValue.trim().toUpperCase();
    // tslint:disable-next-line: no-string-literal
    this.seguimientosFiltrados = this.todosLosSeguimientos.filter( seguimiento => seguimiento.supervisor.toUpperCase().includes(filtro)
                                                || seguimiento.observacion.toUpperCase().includes(filtro)
                                                || seguimiento.detalle.toUpperCase().includes(filtro)
                                                || seguimiento.calificacion.includes(filtro)
                                                || seguimiento.fecha.toString().toUpperCase().includes(filtro) );
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
            this.todosLosSeguimientos = (this.seguimientos$ as unknown) as HistorialSeguimiento[]; // pasaje para poder utilizar el metodo slice
            for (const seguimiento of this.todosLosSeguimientos) {
              seguimiento.detalle = UtilesMensajeria.acomodarString(seguimiento.detalle);
            }
            this.seguimientosFiltrados = this.todosLosSeguimientos;
          }
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

  borrar(id: number) {
    let message = `Está seguro de que quiere borrar el seguimiento?`;
    let dialogData = new ConfirmDialogModel('Confirmar', message);
    let dialogRef = this.dialog.open(DialogoConfirmacionComponent, {data: dialogData});

    dialogRef.afterClosed().subscribe(result => {
      if (result === 'true') {
        this.empleadosService.deleteSeguimiento(id)
        .subscribe(
          (res) => {
            // tslint:disable: no-string-literal
            if (res['status'] === 200 || res['status'] === 204) {
              /* const index = this.todosLosSeguimientos.findIndex( x => x.id === id );
              this.todosLosSeguimientos.splice(index, 1);
              this.seguimientosFiltrados = this.todosLosSeguimientos; */
              this.obtenerSeguimientos();
            } else {
              /*message = `Lo sentimos ha ocurrido un error en el servidor`;
              dialogData = new PopupInformativoModel('Error', message, 'danger');
              dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});*/
            }
            // tslint:enable: no-string-literal
        });
      }
    });
  }

  crearSeguimiento() {
    const dialogConfig = new MatDialogConfig();

    dialogConfig.disableClose = true;
    dialogConfig.autoFocus = true;

    dialogConfig.data = {
        idempleado: this.idempleado,
        idusuario: this.authService.currentUserValue.id,
        idacceso: this.idacceso,
        listaIdHijos: this.authService.currentUserValue.listaIdHijos,
        idsector: this.idsector
    };

    const dialogRef = this.dialog.open(NuevoSeguimientoComponent, dialogConfig);

    dialogRef.afterClosed().subscribe(
      (res) => {
        if (res === 'ok') {
          this.obtenerSeguimientos();
        }
      }
    );
  }

  // función para verificar si tiene permiso para hacer seguimientos
  puedeHacerSeguimiento() {
    switch (this.idacceso) {
      case 1:
      case 3:
      case 9:
      case 10:
      case 13:
      case 18:
      case 21:
      case 22:
        this.permisoSeguimiento = true;
        break;
      default:
        this.permisoSeguimiento = false;
    }
  }

}
