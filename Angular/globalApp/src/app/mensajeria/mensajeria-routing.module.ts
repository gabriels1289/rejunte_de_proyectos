import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';

import { MensajeriaComponent } from './mensajeria/mensajeria.component';
import { BandejaDeEntradaComponent } from './bandeja-de-entrada/bandeja-de-entrada.component';
import { BandejaDeSalidaComponent } from './bandeja-de-salida/bandeja-de-salida.component';
import { NuevoCorreoComponent } from './nuevo-correo/nuevo-correo.component';
import { BandejaDeEntradaRespuestasComponent } from './bandeja-de-entrada-respuestas/bandeja-de-entrada-respuestas.component';


import { AuthGuard } from '../auth/auth.guard';
import { Rol } from '../_models/roles';
import { CanDeactivateGuard } from '../_helpers/can-deactivate.guard';

const mensajeriaRoutes: Routes = [
  {
    path: 'mensajeria',
    component: MensajeriaComponent,
    canActivate: [AuthGuard],
    children: [
      {
        path: '',
        canActivateChild: [AuthGuard],
        children: [
          { path: 'bandeja-de-entrada', component: BandejaDeEntradaComponent },
          { path: 'bandeja-de-entrada/:idmensaje', component: BandejaDeEntradaRespuestasComponent },
          { path: 'bandeja-de-salida', component: BandejaDeSalidaComponent },
          { path: 'nuevo-correo', component: NuevoCorreoComponent, canDeactivate: [CanDeactivateGuard] },
          { path: 'reenviar', component: NuevoCorreoComponent, canDeactivate: [CanDeactivateGuard] },
          { path: 'responder/:idmensaje', component: NuevoCorreoComponent, canDeactivate: [CanDeactivateGuard] }
        ]
      }
    ]
  },
  /* {
    path: 'mensajeria/bandeja-de-entrada/:idmensaje',
    component: BandejaDeEntradaRespuestasComponent,
    canActivate: [AuthGuard],
  } */
];

@NgModule({
  imports: [RouterModule.forChild(mensajeriaRoutes)],
  exports: [RouterModule]
})
export class MensajeriaRoutingModule { }

// TODO: quitar el parametro de ruta para bandeja de entrada
