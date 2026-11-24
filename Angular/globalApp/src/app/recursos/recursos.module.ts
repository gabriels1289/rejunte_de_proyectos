import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { RecursosRoutingModule } from './recursos-routing.module';
import { SharedModule } from '../utilidades/shared/shared.module';
import { BibliotecaComponent } from './biblioteca/biblioteca.component';
import { GestorBibliotecaComponent } from './gestor-biblioteca/gestor-biblioteca.component';
import { DetalleComponent } from './detalle/detalle.component';
import { EdicionComponent } from './edicion/edicion.component';

@NgModule({
  declarations: [
    BibliotecaComponent,
    GestorBibliotecaComponent,
    DetalleComponent,
    EdicionComponent
  ],
  imports: [
    CommonModule,
    SharedModule,
    RecursosRoutingModule
  ],
  entryComponents: [
    DetalleComponent,
    EdicionComponent
  ]
})
export class RecursosModule { }
