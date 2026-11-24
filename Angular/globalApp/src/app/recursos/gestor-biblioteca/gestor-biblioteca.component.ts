import { Component, OnInit, ViewChild } from '@angular/core';
import { PageEvent, MatDialog, MatPaginator, MatDialogConfig } from '@angular/material';
import { RecursosService } from '../servicios/recursos.service';
import { Router, ActivatedRoute } from '@angular/router';
import { AuthService } from 'src/app/auth/auth.service';
import { PopupInformativoModel, PopupInformativoComponent } from 'src/app/modales/popup-informativo/popup-informativo.component';
import { Utilidades } from 'src/app/utilidades/utilidades';
import { DetalleComponent } from '../detalle/detalle.component';
import { ConfirmDialogModel, DialogoConfirmacionComponent } from 'src/app/modales/dialogo-confirmacion/dialogo-confirmacion.component';
import { EdicionComponent } from '../edicion/edicion.component';

@Component({
  selector: 'app-gestor-biblioteca',
  templateUrl: './gestor-biblioteca.component.html',
  styleUrls: ['./gestor-biblioteca.component.css']
})
export class GestorBibliotecaComponent implements OnInit {

  // Lo agregue para que no de error al compilar, es para pasar como parametro de una funcion
  public variableNull = null;

  public spinner = true; // arranca con el spinner hasta que obtiene los datos del servidor
  // MatPaginator Inputs
  pageSize = 10;
  pageSizeOptions: number[] = [5, 10, 25, 50, 100];
  // MatPaginator Output
  pageEvent: PageEvent;
  pageIndex = 0;

  idusuario: number;

  displayedColumns: string[] = ['id', 'titulo', 'comentario', 'menu'];
  dataSource;

  @ViewChild(MatPaginator, {static: true}) paginator: MatPaginator;
  todosLosRecursos = [];
  recursosFiltrados = [];
  estados = [
    {value: 'A', viewValue: 'Activos'},
    {value: 'N', viewValue: 'Dados de baja'},
    {value: 'TODoS', viewValue: 'Todos'},
  ];
  public selectedValue: string;
  valorFiltro = '';

  constructor(
    private recursosService: RecursosService,
    private router: Router,
    private rutaActiva: ActivatedRoute,
    private authService: AuthService,
    public dialog: MatDialog
  ) {
    this.idusuario = this.authService.currentUserValue.id;
  }

  ngOnInit() {
    this.obtenerRecursos();
  }

  obtenerRecursos() {
    this.selectedValue = 'A';
    this.recursosService.getListadoDeRecursos(this.idusuario)
    .subscribe(
      (res) => {
        // tslint:disable: no-string-literal
        // console.log(res);
        // console.log('controlador: %o', res['body']);
        if (res['status'] === 200 || res['status'] === 204) {
          if (res['status'] === 200) {
            this.todosLosRecursos = res['body'];
            this.recursosFiltrados = this.todosLosRecursos.filter( recurso => recurso.activo === 'A');
          }
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

  //#region paginador
  // función para setear el tamaño de la paginación
  setPageSizeOptions(setPageSizeOptionsInput: string) {
    this.pageSizeOptions = setPageSizeOptionsInput.split(',').map(str => +str);
  }

  public handlePage(e: PageEvent): PageEvent {
    this.pageSize = e.pageSize;
    this.pageIndex = e.pageIndex;
    return e;
  }
  //#endregion paginador

  applyFilter(filterValue: string) {
    // this.todosLosEmpleados.filter = filterValue.trim().toLowerCase();
    const filtro = filterValue.trim().toUpperCase();

    let auxFiltroActivos;
    switch (this.selectedValue) {
      case 'A':
        auxFiltroActivos = this.todosLosRecursos.filter( recurso => recurso.activo === 'A');
        break;
      case 'N':
        auxFiltroActivos = this.todosLosRecursos.filter( recurso => recurso.activo === 'N');
        break;
      default:
        auxFiltroActivos = this.todosLosRecursos;
        break;
    }

    // tslint:disable-next-line: no-string-literal
    this.recursosFiltrados = auxFiltroActivos.filter( recurso => recurso.areas.includes(filtro) );
  }

  onChangeOption() {
    // TODO: amostrar el desplegable segun permisos y acomodar la presentacion
    this.valorFiltro = '';
    let auxFiltroActivos;
    switch (this.selectedValue) {
      case 'A':
        auxFiltroActivos = this.todosLosRecursos.filter( recurso => recurso.activo === 'A');
        break;
      case 'N':
        auxFiltroActivos = this.todosLosRecursos.filter( recurso => recurso.activo === 'N');
        break;
      default:
        auxFiltroActivos = this.todosLosRecursos;
        break;
    }
    this.recursosFiltrados = auxFiltroActivos;
  }

  verDetalle(recurso) {
    this.dialog.open(DetalleComponent, {data: recurso});
  }

  altaBaja(recurso) {
    const message = `Está seguro de que quiere hacerlo?`;
    const dialogData = new ConfirmDialogModel('Confirmar', message);
    const dialogRef = this.dialog.open(DialogoConfirmacionComponent, {data: dialogData});

    dialogRef.afterClosed().subscribe(result => {
      if (result === 'true') {

        const data = {
          idrecurso: recurso.id,
          activo: recurso.activo
        };

        this.recursosService.putAltaBaja(JSON.stringify(data)).subscribe(
          (res) => {
            // tslint:disable-next-line: no-string-literal
            if (res['status'] === 200) {
              if (recurso.activo === 'A') {
                recurso.activo = 'N';
              } else {
                recurso.activo = 'A';
              }
            }
          }
        );
      }
    });
  }



  // de momento no esta en uso por el tema del directorio donde estan los archivos
  borrar(recurso) {
    const message = `Está seguro de que quiere hacerlo?`;
    const dialogData = new ConfirmDialogModel('Confirmar', message);
    const dialogRef = this.dialog.open(DialogoConfirmacionComponent, {data: dialogData});

    dialogRef.afterClosed().subscribe(result => {
      if (result === 'true') {

        const data = {
          idrecurso: recurso.id,
          tipo: recurso.tipo,
          direccion: recurso.direccion
        };

        this.recursosService.deleteRecurso(JSON.stringify(data)).subscribe(
          (res) => {
            // tslint:disable-next-line: no-string-literal
            if (res['status'] === 200) {
              this.obtenerRecursos();
            }
          }
        );
      }
    });
  }

  cargarEditar(recurso) {
    let aux;
    if (recurso == null) {
      // es una carga
      const rec = {
        id: 0,
        titulo: '',
        areas: '',
        comentario: '',
        tipo: 'V',
        direccion: ''
      };
      aux = rec;
    } else {
      // es una edicion
      aux = recurso;
    }
    // console.log('Aux: %o', aux);
    // console.log('Split: %o', aux.areas.split(','));

    const dialogConfig = new MatDialogConfig();

    dialogConfig.disableClose = true;
    dialogConfig.autoFocus = true;

    dialogConfig.data = aux;

    const dialogRef = this.dialog.open(EdicionComponent, dialogConfig);

    dialogRef.afterClosed().subscribe(
      (res) => {
        if (res === 'ok') {
          this.obtenerRecursos();
        }
      }
    );
  }

}
