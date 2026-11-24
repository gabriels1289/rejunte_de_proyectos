import { Component, OnInit, Inject } from '@angular/core';
import { MAT_DIALOG_DATA } from '@angular/material';

@Component({
  selector: 'app-detalle',
  templateUrl: './detalle.component.html',
  styleUrls: ['./detalle.component.css']
})
export class DetalleComponent implements OnInit {
  titulo: string;
  areas: string;
  comentario: string;
  tipo: string;
  direccion: string;

  constructor(@Inject(MAT_DIALOG_DATA) public data: any) {
    this.titulo = data.titulo;
    this.areas = data.areas;
    this.comentario = data.comentario;
    this.tipo = data.tipo;
    this.direccion = data.direccion;
  }

  ngOnInit() {
    if (this.tipo === 'V') {
      this.direccion = this.direccion.replace('https://www.youtube.com/watch?v=', 'https://www.youtube.com/embed/');
      if (!this.direccion.endsWith('?rel=0')) {
        this.direccion += '&showinfo=0&iv_load_policy=3?rel=0'; // evita sugerencias de otros videos al terminar la reproduccion
      }
    }
  }

}
