import { Component, OnInit, ViewChild } from '@angular/core';
import { EmpleadosService } from '../servicios/empleados.service';
import { ActivatedRoute, Router } from '@angular/router';
import { AuthService } from 'src/app/auth/auth.service';
import { Empleado, EmpleadoListado } from '../comunes/empleado';
import { PopupInformativoModel, PopupInformativoComponent } from 'src/app/modales/popup-informativo/popup-informativo.component';
import { MatDialog, PageEvent, MatTableDataSource, MatPaginator, MatSort, MatSortable } from '@angular/material';
import { Observable } from 'rxjs';

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

  private empleados$: Observable<EmpleadoListado[]>;
  public todosLosEmpleados: EmpleadoListado[]; // para almacenar los observables como array y usar el metodo slice en la paginacion
  public empleadosFiltrados: EmpleadoListado[] = [];
  public spinner = true; // arranca con el spinner hasta que obtiene los datos del servidor
  selectedId: number;
  estados: Estado[] = [
    {value: 'SI', viewValue: 'Activos'},
    {value: 'NO', viewValue: 'Dados de baja'},
    {value: 'TODOS', viewValue: 'Todos'},
  ];
  public selectedValue: string;
  private idusuario;
  private listahijos;
  private idacceso;
  public valorFiltro = '';

  // MatPaginator Inputs
  pageSize = 25;
  pageSizeOptions: number[] = [5, 10, 25, 50, 100];
  // MatPaginator Output
  pageEvent: PageEvent;
  pageIndex = 0;

  displayedColumns: string[] = ['foto', 'id', 'legajo', 'apellido', 'nombre', 'sector'];
  dataSource;

  @ViewChild(MatPaginator, {static: true}) paginator: MatPaginator;
  // @ViewChild(MatSort, {static: true}) sort: MatSort;

  constructor(
    private empleadosService: EmpleadosService,
    private rutaActiva: ActivatedRoute,
    private authService: AuthService,
    private router: Router,
    public dialog: MatDialog
  ) { }

  ngOnInit() {
    this.idusuario = this.authService.currentUserValue.id;
    this.idacceso = this.authService.currentUserValue.idacceso;
    this.listahijos = this.authService.currentUserValue.listaIdHijos;
    this.selectedValue = 'SI';
    // TODO: evitar que se obtenga en el listado?
    this.empleadosService.getListadoDeEmpleados(this.idacceso, this.listahijos)
    .subscribe(
      (res) => {
        // console.log(res);
        // tslint:disable: no-string-literal
        if (res['status'] === 200 || res['status'] === 204) {
          if (res['status'] === 200) {
            this.empleados$ = res['body'];
            this.todosLosEmpleados = (this.empleados$ as unknown) as EmpleadoListado[]; // pasaje para poder utilizar el metodo slice
            this.empleadosFiltrados = this.todosLosEmpleados.filter( empleado => empleado.activo === 'SI');
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
    // this.todosLosEmpleados.filter = filterValue.trim().toLowerCase();
    const filtro = filterValue.trim().toUpperCase();

    let auxFiltroActivos;
    switch (this.selectedValue) {
      case 'SI':
        auxFiltroActivos = this.todosLosEmpleados.filter( empleado => empleado.activo === 'SI');
        break;
      case 'NO':
        auxFiltroActivos = this.todosLosEmpleados.filter( empleado => empleado.activo === 'NO');
        break;
      default:
        auxFiltroActivos = this.todosLosEmpleados;
        break;
    }

    // tslint:disable-next-line: no-string-literal
    this.empleadosFiltrados = auxFiltroActivos.filter( empleado => empleado.apellido.includes(filtro)
                                                || empleado.nombre.includes(filtro)
                                                || empleado.sector.includes(filtro)
                                                || empleado.id.toString().includes(filtro)
                                                || empleado.legajo.toString().includes(filtro) );
  }

  onRowClicked(row) {
    // tslint:disable-next-line: max-line-length
    this.router.navigate(['empleados/' + row.id + '/' + row.nombre + '/' + row.apellido + '/' + row.idsector + '/' + row.sector + '/seguimientos']);
  }

  onChangeOption() {
    // TODO: mostrar el desplegable segun permisos
    this.valorFiltro = '';
    let auxFiltroActivos;
    switch (this.selectedValue) {
      case 'SI':
        auxFiltroActivos = this.todosLosEmpleados.filter( empleado => empleado.activo === 'SI');
        break;
      case 'NO':
        auxFiltroActivos = this.todosLosEmpleados.filter( empleado => empleado.activo === 'NO');
        break;
      default:
        auxFiltroActivos = this.todosLosEmpleados;
        break;
    }
    this.empleadosFiltrados = auxFiltroActivos;
  }

  nuevoEmpleado() {
    this.router.navigate(['empleados/nuevo-empleado']);
  }

  get permisoNuevoEmpleado() {
    if (this.idacceso === 1 || this.idacceso === 3 || this.idacceso === 10) {
      return true;
    } else {
      return false;
    }
  }

}
