import { NgModule } from '@angular/core';
import { Routes, RouterModule, PreloadAllModules } from '@angular/router';
import { AuthGuard } from './auth/auth.guard';
import { SelectivePreloadingStrategyService } from './_helpers/selective-preloading-strategy.service';
import { Rol } from './_models/roles';

const appRoutes: Routes = [
  {
    path: 'admin',
    loadChildren: () => import('./admin/admin.module').then(mod => mod.AdminModule),
    canLoad: [AuthGuard],
    data: { roles: [Rol.Sistemas, Rol.ControlDeJuegoIII] }
  },
  {
    path: 'reportes',
    loadChildren: () => import('./reportes/reportes.module').then(mod => mod.ReportesModule),
    data: { preload: true}
  },
  /*{
    path: 'mensajeria',
    loadChildren: () => import('./mensajeria/mensajeria.module').then(mod => mod.MensajeriaModule),
    canLoad: [AuthGuard]
  }*/
  // TODO: pasar a este modulo empleados y dejar como carga inicial el modulo de mensajeria
  { path: '', redirectTo: '/mensajeria/bandeja-de-entrada', pathMatch: 'full' },
  // redigir en caso de hallar la ruta
  { path: '**', redirectTo: '/mensajeria/bandeja-de-entrada' }
];

@NgModule({
  imports: [RouterModule.forRoot(appRoutes, { preloadingStrategy: SelectivePreloadingStrategyService })],
  exports: [RouterModule]
})
export class AppRoutingModule { }
