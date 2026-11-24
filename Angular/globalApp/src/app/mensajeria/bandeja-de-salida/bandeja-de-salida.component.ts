import { Component, OnInit } from '@angular/core';
import { MensajeriaService } from '../servicios/mensajeria.service';
import { ActivatedRoute, Router } from '@angular/router';

import { Observable } from 'rxjs';

import { Correo } from '../comunes/correo';
import { AuthService } from '../../auth/auth.service';

import {PageEvent} from '@angular/material/paginator';

import { MatDialog } from '@angular/material';
import { ModalSalidaComponent } from '../modales/modal-salida/modal-salida.component';
import { DialogoConfirmacionComponent, ConfirmDialogModel } from '../../modales/dialogo-confirmacion/dialogo-confirmacion.component';
import { PopupInformativoModel, PopupInformativoComponent } from 'src/app/modales/popup-informativo/popup-informativo.component';

@Component({
  selector: 'app-bandeja-de-salida',
  templateUrl: './bandeja-de-salida.component.html',
  styleUrls: ['./bandeja-de-salida.component.css']
})
export class BandejaDeSalidaComponent implements OnInit {

  // la agregue por el error del compilador
  public last;

  private correos$: Observable<Correo[]>;
  public todosLosCorreos: Correo[]; // pasaje para poder utilizar el metodo slice en la paginación
  public spinner = true; // arranca con el spinner hasta que obtiene los datos del servidor
  selectedId: number;

  // MatPaginator Inputs
  pageSize = 25;
  pageSizeOptions: number[] = [5, 10, 25, 50, 100];
  // MatPaginator Output
  pageEvent: PageEvent;
  pageIndex = 0;

  constructor(
    private mensajeriaService: MensajeriaService,
    private rutaActiva: ActivatedRoute,
    private authService: AuthService,
    private router: Router,
    public dialog: MatDialog
    ) { }

  ngOnInit() {
    const idusuario = this.authService.currentUserValue.id;
    this.mensajeriaService.getBandejaDeSalida(idusuario).subscribe(
      (res) => {
        // console.log(res);
        // tslint:disable: no-string-literal
        if (res['status'] === 200 || res['status'] === 204) {
          this.correos$ = res['body'];
          this.todosLosCorreos = this.correos$ as unknown as Correo[]; // hago este pasaje para poder utilizar el metodo slice
          this.spinner = false;
        } else {
          /*const message = `Lo sentimos ha ocurrido un error en el servidor`;
          const dialogData = new PopupInformativoModel('Error', message, 'danger');
          const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});*/
          this.spinner = false;
        }
        // tslint:enable: no-string-literal
      }
    );

    this.rutaActiva.queryParamMap.subscribe(
      params => {
        this.pageIndex = +params.get('pagina') || 0;
        this.selectedId = +params.get('idcorreo') || 0;
      }
    );
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

  public detalle(correo: Correo) {
    // veo quien lo creo y selecciono la tabla destino para quitar en caso de ser necesario el destacado
    let auxTabla;
    if (correo.esRespuesta === 'M') {
      auxTabla = 'mensajes';
    } else {
      auxTabla = 'area_mensaje';
    }

    // hago el get http del detalle y luego un openDialog()
    this.mensajeriaService.getBandejaDeSalidaDetalle(correo.id, auxTabla, this.authService.currentUserValue.id).subscribe(
      (res) => {
        // tslint:disable-next-line: no-string-literal
        if (res['status'] === 200) {
          // tslint:disable-next-line: no-string-literal
          const detalle = res['body'];
          detalle[0].destacado = 'N';
          this.dialog.open(ModalSalidaComponent, {data: detalle});
        }
      }
    );
  }

  public destacar(correo: Correo) {
    const message = `Está seguro de que quiere hacerlo?`;
    const dialogData = new ConfirmDialogModel('Confirmar', message);
    const dialogRef = this.dialog.open(DialogoConfirmacionComponent, {data: dialogData});

    dialogRef.afterClosed().subscribe(result => {
      if (result === 'true') {
        this.mensajeriaService.setPrioridad(correo.id).subscribe(
          (res) => {
            // tslint:disable-next-line: no-string-literal
            if (res['status'] === 200) {
              correo.destacado = 'I';
            }
          }
        );
      }
    });
  }

  public goToReenviar(correo: Correo) {
    let auxTabla;
    if (correo.esRespuesta === 'M') {
      auxTabla = 'mensajes';
    } else {
      auxTabla = 'respuestas';
    }
    this.router.navigate(['/mensajeria/reenviar'], { queryParams: { idmensaje: correo.id, tabla: auxTabla }});
  }

}
