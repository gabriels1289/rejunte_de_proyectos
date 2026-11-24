import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { AuthGuard } from '../auth/auth.guard';
import { BibliotecaComponent } from './biblioteca/biblioteca.component';
import { GestorBibliotecaComponent } from './gestor-biblioteca/gestor-biblioteca.component';

const recursosRoutes: Routes = [
  { path: 'biblioteca', component: BibliotecaComponent, canActivate: [AuthGuard] },
  { path: 'gestor-biblioteca', component: GestorBibliotecaComponent, canActivate: [AuthGuard]}
];

@NgModule({
  imports: [RouterModule.forChild(recursosRoutes)],
  exports: [RouterModule]
})
export class RecursosRoutingModule { }
