import { Component, OnInit, ViewChild } from '@angular/core';
import { PageEvent, MatPaginator, MatDialog } from '@angular/material';
import { MisAjustesService } from '../servicios/mis-ajustes.service';
import { ActivatedRoute, Router } from '@angular/router';
import { AuthService } from 'src/app/auth/auth.service';
import { PopupInformativoModel, PopupInformativoComponent } from 'src/app/modales/popup-informativo/popup-informativo.component';

export interface Estado {
  value: string;
  viewValue: string;
}

@Component({
  selector: 'app-logs',
  templateUrl: './logs.component.html',
  styleUrls: ['./logs.component.css']
})
export class LogsComponent implements OnInit {

  public spinner = true; // arranca con el spinner hasta que obtiene los datos del servidor
  public todosLosRegistros: any[]; // para almacenar los observables como array y usar el metodo slice en la paginacion
  public registrosFiltrados: any[] = [];

  estados: Estado[] = [
    {value: 'FATAL', viewValue: 'Fatal'},
    {value: 'ERROR', viewValue: 'Error'},
    {value: 'WARN', viewValue: 'Warning'},
    {value: 'INFO', viewValue: 'Info'},
    {value: 'DEBUG', viewValue: 'Debug'},
    {value: 'TRACE', viewValue: 'Trace'},
    {value: 'IMPORTANTES', viewValue: 'Importantes'},
    {value: 'TODOS', viewValue: 'Todos'}
  ];
  public selectedValue: string;
  private idusuario;
  private idacceso;
  valorFiltro = '';
  cantidadDeRegistros = 1500;
  noDeslogueados = 0;
  totalActivos = 0;
  porcentajeOnLine = 0;

  // MatPaginator Inputs
  pageSize = 25;
  pageSizeOptions: number[] = [5, 10, 25, 50, 100];
  // MatPaginator Output
  pageEvent: PageEvent;
  pageIndex = 0;

  displayedColumns: string[] = ['id', 'nivel', 'usuario', 'nombre', 'mensaje', 'fecha', 'archivo', 'linea'];

  @ViewChild(MatPaginator, {static: true}) paginator: MatPaginator;

  constructor(
    private ajustesService: MisAjustesService,
    private rutaActiva: ActivatedRoute,
    private authService: AuthService,
    private router: Router,
    public dialog: MatDialog
  ) { }

  ngOnInit() {
    this.idusuario = this.authService.currentUserValue.id;
    this.idacceso = this.authService.currentUserValue.idacceso;
    this.selectedValue = 'TODOS';
    this.obtenerLogs();
  }

  obtenerLogs() {
    this.spinner = true;
    this.ajustesService.getLogs(this.cantidadDeRegistros)
    .subscribe(
      (res) => {
        // console.log(res);
        // tslint:disable: no-string-literal
        if (res['status'] === 200) {
          this.todosLosRegistros = res['body'][0];
          this.registrosFiltrados = this.todosLosRegistros;
          /* this.registrosFiltrados = this.todosLosRegistros.filter( registro => registro.nivel === 'FATAL'
                                                                                || registro.nivel === 'ERROR'
                                                                                || registro.nivel === 'WARN'); */
          this.noDeslogueados = res['body'][1][0].noDeslogueados;
          this.totalActivos = res['body'][2][0].totalActivos;
          this.porcentajeOnLine = (this.noDeslogueados / this.totalActivos);
          // tslint:enable: no-string-literal
          this.spinner = false;
        } else {
         /* const message = `Lo sentimos ha ocurrido un error en el servidor`;
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
      case 'FATAL':
        auxFiltroActivos = this.todosLosRegistros.filter( registro => registro.nivel === 'FATAL');
        break;
      case 'ERROR':
        auxFiltroActivos = this.todosLosRegistros.filter( registro => registro.nivel === 'ERROR');
        break;
      case 'WARN':
        auxFiltroActivos = this.todosLosRegistros.filter( registro => registro.nivel === 'WARN');
        break;
      case 'INFO':
        auxFiltroActivos = this.todosLosRegistros.filter( registro => registro.nivel === 'INFO');
        break;
      case 'DEBUG':
        auxFiltroActivos = this.todosLosRegistros.filter( registro => registro.nivel === 'DEBUG');
        break;
      case 'TRACE':
        auxFiltroActivos = this.todosLosRegistros.filter( registro => registro.nivel === 'TRACE');
        break;
      case 'IMPORTANTES':
        auxFiltroActivos = this.todosLosRegistros.filter( registro => registro.nivel === 'FATAL'
                                                                    || registro.nivel === 'ERROR'
                                                                    || registro.nivel === 'WARN');
        break;
      default:
        auxFiltroActivos = this.todosLosRegistros;
        break;
    }

    // tslint:disable-next-line: no-string-literal
    this.registrosFiltrados = auxFiltroActivos.filter( registro =>  registro.id.toString().includes(filtro)
                                                || registro.nivel.toUpperCase().includes(filtro)
                                                || registro.usuario.toString().includes(filtro)
                                                || registro.nombre.includes(filtro)
                                                || registro.mensaje.toUpperCase().includes(filtro)
                                                || registro.fecha.includes(filtro)
                                                || registro.archivo.toUpperCase().includes(filtro)
                                                || registro.linea.toString().includes(filtro) );
  }

  onChangeOption() {
    this.valorFiltro = '';
    let auxFiltroActivos;
    switch (this.selectedValue) {
      case 'FATAL':
        auxFiltroActivos = this.todosLosRegistros.filter( registro => registro.nivel === 'FATAL');
        break;
      case 'ERROR':
        auxFiltroActivos = this.todosLosRegistros.filter( registro => registro.nivel === 'ERROR');
        break;
      case 'WARN':
        auxFiltroActivos = this.todosLosRegistros.filter( registro => registro.nivel === 'WARN');
        break;
      case 'INFO':
        auxFiltroActivos = this.todosLosRegistros.filter( registro => registro.nivel === 'INFO');
        break;
      case 'DEBUG':
        auxFiltroActivos = this.todosLosRegistros.filter( registro => registro.nivel === 'DEBUG');
        break;
      case 'TRACE':
        auxFiltroActivos = this.todosLosRegistros.filter( registro => registro.nivel === 'TRACE');
        break;
      case 'IMPORTANTES':
        auxFiltroActivos = this.todosLosRegistros.filter( registro => registro.nivel === 'FATAL'
                                                                    || registro.nivel === 'ERROR'
                                                                    || registro.nivel === 'WARN');
        break;
      default:
        auxFiltroActivos = this.todosLosRegistros;
        break;
    }
    this.registrosFiltrados = auxFiltroActivos;
  }

  refrescar() {
    this.obtenerLogs();
  }

}
