import { Component, OnInit, Inject } from '@angular/core';
import { MAT_DIALOG_DATA } from '@angular/material';
import { Correo, UtilesMensajeria } from '../../comunes/correo';

@Component({
  selector: 'app-modal-salida',
  templateUrl: './modal-salida.component.html',
  styleUrls: ['./modal-salida.component.css']
})
export class ModalSalidaComponent implements OnInit {

  // la agregue por el error del compilador
  public $last;

  public detalle: Correo;
  public destinatarios;
  public adjuntos;
  public textoDestinatarios;

  constructor(@Inject(MAT_DIALOG_DATA) public data: any) {
    this.detalle = data[0];
    this.destinatarios = data[1];
    this.adjuntos = data[2];
    this.textoDestinatarios = UtilesMensajeria.concatenarDestinatarios(this.destinatarios);
  }

  ngOnInit() {
  }

}
