import { Component, OnInit } from '@angular/core';
import { PageEvent, MatDialog } from '@angular/material';
import { RecursosService } from '../servicios/recursos.service';
import { Router, ActivatedRoute } from '@angular/router';
import { AuthService } from 'src/app/auth/auth.service';
import { PopupInformativoModel, PopupInformativoComponent } from 'src/app/modales/popup-informativo/popup-informativo.component';

@Component({
  selector: 'app-biblioteca',
  templateUrl: './biblioteca.component.html',
  styleUrls: ['./biblioteca.component.css']
})
export class BibliotecaComponent implements OnInit {

  public spinner = true; // arranca con el spinner hasta que obtiene los datos del servidor
  // MatPaginator Inputs
  pageSize = 10;
  pageSizeOptions: number[] = [5, 10, 25, 50, 100];
  // MatPaginator Output
  pageEvent: PageEvent;
  pageIndex = 0;

  // MatPaginator Inputs
  pageSize2 = 10;
  pageSizeOptions2: number[] = [5, 10, 25, 50, 100];
  // MatPaginator Output
  pageEvent2: PageEvent;
  pageIndex2 = 0;

  idusuario: number;
  idacceso: number;
  idsector: number;
  videos: any;
  documentos: any;
  actual: any;

  constructor(
    private recursosService: RecursosService,
    private router: Router,
    private rutaActiva: ActivatedRoute,
    private authService: AuthService,
    public dialog: MatDialog
  ) {
    this.idusuario = this.authService.currentUserValue.id;
    this.idacceso = this.authService.currentUserValue.idacceso;
    this.idsector = this.authService.currentUserValue.idarea;
  }

  ngOnInit() {
    this.cargarBiblioteca();
  }

  cargarBiblioteca() {
    this.recursosService.getBiblioteca(this.idsector)
    .subscribe(
      (res) => {
        // console.log(res);
        // tslint:disable: no-string-literal
        if (res['status'] === 200) {
          if (res['body'][0] !== 'vacio') {
            this.videos = res['body'][0];
          }
          if (res['body'][1] !== 'vacio') {
            this.documentos = res['body'][1];
          }
          this.spinner = false;
        } else if (res['status'] === 204) {
          const message = `No se encontró la plantilla`;
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

  // función para setear el tamaño de la paginación
  setPageSizeOptions2(setPageSizeOptionsInput: string) {
    this.pageSizeOptions2 = setPageSizeOptionsInput.split(',').map(str => +str);
  }

  public handlePage2(e: PageEvent): PageEvent {
    this.pageSize2 = e.pageSize;
    this.pageIndex2 = e.pageIndex;
    return e;
  }
  //#endregion

  visualizar(embeber) {
    if (embeber.tipo === 'V') {
      embeber.direccion = embeber.direccion.replace('https://www.youtube.com/watch?v=', 'https://www.youtube.com/embed/');
      if (!embeber.direccion.endsWith('?rel=0')) {
        embeber.direccion += '&showinfo=0&iv_load_policy=3?rel=0'; // evita sugerencias de otros videos al terminar la reproduccion
      }
    }
    this.actual = embeber;
    /* if (this.actual.tipo === 'D') {
      $window.open(embeber.direccion);
    } */
  }

  gestor() {
    this.router.navigate(['gestor-biblioteca']);
  }

  get permisoGestor() {
    if (this.idsector === 1 || this.idsector === 15) {
      return true;
    } else {
      return false;
    }
  }

}
