import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { SearchComponent } from '../search/search.component';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { MatListModule, MatBadgeModule, MatButtonModule, MatPaginatorModule, MatIconModule,
  MatProgressSpinnerModule, MatTooltipModule, MatExpansionModule, MatDialogModule, MatPaginatorIntl,
  MatFormFieldModule, MatInputModule, MatChipsModule, MatAutocompleteModule, MatSlideToggleModule,
  MatProgressBarModule, MatSelectModule, MatTableModule, MatSortModule, MatDatepickerModule, MatNativeDateModule,
  MatCheckboxModule } from '@angular/material';
import { PaginadorPipe } from '../../pipes/paginador.pipe';
import { SafeHtmlPipe } from '../../pipes/safe-html.pipe';
import { SafeResourseUrlPipe } from '../../pipes/safe-resourse-url.pipe';

@NgModule({
  declarations: [
    SearchComponent,
    PaginadorPipe,
    SafeHtmlPipe,
    SafeResourseUrlPipe
  ],
  exports: [
    MatTableModule,
    MatSortModule,
    SearchComponent,
    MatListModule,
    MatBadgeModule,
    MatButtonModule,
    MatPaginatorModule,
    MatIconModule,
    MatProgressSpinnerModule,
    MatTooltipModule,
    MatExpansionModule,
    MatDialogModule,
    FormsModule,
    ReactiveFormsModule,
    MatFormFieldModule,
    MatInputModule,
    MatChipsModule,
    MatAutocompleteModule,
    MatSlideToggleModule,
    MatProgressBarModule,
    MatSelectModule,
    MatDatepickerModule,
    MatNativeDateModule,
    MatCheckboxModule,
    PaginadorPipe,
    SafeHtmlPipe,
    SafeResourseUrlPipe
  ],
  imports: [
    CommonModule,
    ReactiveFormsModule,
    FormsModule,
    MatTableModule,
    MatSortModule,
    MatListModule,
    MatBadgeModule,
    MatButtonModule,
    MatPaginatorModule,
    MatIconModule,
    MatProgressSpinnerModule,
    MatTooltipModule,
    MatExpansionModule,
    MatDialogModule,
    FormsModule,
    ReactiveFormsModule,
    MatFormFieldModule,
    MatInputModule,
    MatChipsModule,
    MatAutocompleteModule,
    MatSlideToggleModule,
    MatProgressBarModule,
    MatSelectModule,
    MatDatepickerModule,
    MatNativeDateModule,
    MatCheckboxModule
  ]
})
export class SharedModule { }
