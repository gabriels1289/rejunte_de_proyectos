import { Component, OnInit, Inject } from '@angular/core';
import { MAT_DIALOG_DATA } from '@angular/material';

@Component({
  selector: 'app-popup-informativo',
  templateUrl: './popup-informativo.component.html',
  styleUrls: ['./popup-informativo.component.css']
})
export class PopupInformativoComponent implements OnInit {
  titulo: string;
  mensaje: string;
  tipo: string;

  constructor(@Inject(MAT_DIALOG_DATA) public data: PopupInformativoModel) {
    // Update view with given values
    this.titulo = data.titulo;
    this.mensaje = data.mensaje;
    this.tipo = data.tipo;
  }

  ngOnInit() {
  }

}

/**
 * Class to represent confirm dialog model.
 *
 * It has been kept here to keep it as part of shared component.
 */
export class PopupInformativoModel {

  constructor(public titulo: string, public mensaje: string, public tipo: string) {
  }
}
