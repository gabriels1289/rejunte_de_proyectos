import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';

import { MensajeriaRoutingModule } from './mensajeria-routing.module';
import { MensajeriaComponent } from './mensajeria/mensajeria.component';
import { BandejaDeEntradaComponent } from './bandeja-de-entrada/bandeja-de-entrada.component';
import { BandejaDeSalidaComponent } from './bandeja-de-salida/bandeja-de-salida.component';
import { NuevoCorreoComponent } from './nuevo-correo/nuevo-correo.component';

import { MensajeriaService } from './servicios/mensajeria.service';
import { BandejaDeEntradaRespuestasComponent } from './bandeja-de-entrada-respuestas/bandeja-de-entrada-respuestas.component';

import { CustomMatPaginatorIntl } from './comunes/paginador-es';
import { ModalEntradaComponent } from './modales/modal-entrada/modal-entrada.component';
import { ModalSalidaComponent } from './modales/modal-salida/modal-salida.component';
import { SharedModule } from '../utilidades/shared/shared.module';
import { CKEditorModule } from 'ng2-ckeditor';
import { MatPaginatorIntl } from '@angular/material';

@NgModule({
  declarations: [
    MensajeriaComponent,
    BandejaDeEntradaComponent,
    BandejaDeSalidaComponent,
    NuevoCorreoComponent,
    BandejaDeEntradaRespuestasComponent,
    ModalEntradaComponent,
    ModalSalidaComponent
  ],
  entryComponents: [
    ModalEntradaComponent,
    ModalSalidaComponent
  ],
  imports: [
    CommonModule,
    MensajeriaRoutingModule,
    SharedModule,
    FormsModule,
    ReactiveFormsModule,
    CKEditorModule
  ],
  providers: [
    MensajeriaService,
    {
      provide: MatPaginatorIntl,
      useClass: CustomMatPaginatorIntl
    }
  ]
})
export class MensajeriaModule { }
